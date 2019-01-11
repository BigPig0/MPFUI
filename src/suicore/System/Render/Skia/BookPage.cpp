
#include "bookpage.h"
#include <math.h>
#include <System/Tools/Math.h>

BookPage::BookPage()
{
    _width = 0;
    _height = 0;
    _cornerX = 0;
    _cornerY = 0;
    _curPage = NULL;
    _nextPage = NULL;

    _maxLength = 0;

    _touchPoint.fX = 0.01f;
    _touchPoint.fY = 0.01f;

    for (int i = 0; i < 9; i++)
    {
        _matrixArray[i] = 0;
    }

    _matrixArray[8] = 1.0f;
}

BookPage::~BookPage(void)
{
    ClearBookPage();
}

void BookPage::ClearBookPage()
{
    if (_colorMatrixFilter)
    {
        delete _colorMatrixFilter;
        _colorMatrixFilter = NULL;
    }
}

void BookPage::InitBookPage(int w, int h)
{
    _width = 800;
    _height = 480;
    _cornerX = 0;
    _cornerY = 0;
    _curPage = NULL;
    _nextPage = NULL;

    _maxLength = _hypot(_width, _height);

    CreateShader();
    _paint.setStyle(SkPaint::Style::kFill_Style);

    // SkColorMatrix cm;

    float farray[] = {0.55f, 0, 0, 0, 80.0f,    // 0-4
                      0, 0.55f, 0, 0, 80.0f,    // 5-9
                      0, 0, 0.55f,0, 80.0f,     // 10-14
                      0, 0, 0, 0.2f, 0};        // 15-19

    //ԭ��������farray��ʼ��ColorMatrix,����
    //ColorMatrix��ʼ��ColorMatrixColorFilter��
    //����ֱ��������farray��ʼ��SkColorMatrixFilter
    //ColorMatrix��5*4���󣿷ֱ����RGBA��Scale��
    _colorMatrixFilter = SkColorMatrixFilter::Create(farray);

    // ����x,yΪ0,�����ڵ����ʱ��������
    _touchPoint.fX = 0.01f;
    _touchPoint.fY = 0.01f;

    for (int i = 0; i < 9; i++)
    {
        _matrixArray[i] = 0;
    }

    _matrixArray[8] = 1.0f;
}

// ������Ӱ��GradientDrawable
void BookPage::CreateShader()
{
    SkPoint pt[2];
    pt[0].set(0,0);
    pt[1].set(50,50);
    SkColor color[] = {0x888888 ,0xb0333333};
    mFolderShadowDrawableRL = SkGradientShader::CreateLinear(pt,color,NULL,2,SkShader::TileMode::kClamp_TileMode);
}

// ������ק���Ӧ����ק��
void BookPage::CalcCornerXY(Float x,Float y)
{
    if (x <= _width / 2.0f)
    {
        _cornerX = 0;
    }
    else
    {
        _cornerX =_width;
    }

    if (y <= _height / 2.0f)
    {
        _cornerY =0;
    }
    else
    {
        _cornerY =_height;
    }

    if ((0 == _cornerX && _cornerY == _height)      // ���½�
        ||(_cornerX == _width && 0 == _cornerY))    // ���Ͻ�
    {
        mIsRTandLB = TRUE;
    }
    else
    {
        mIsRTandLB = FALSE;
    }
}

SkPoint BookPage::GetCross(SkPoint p1, SkPoint p2, SkPoint p3, SkPoint p4)
{
    SkPoint CrossP ;
    Float a1 = (p2.fY - p1.fY) / (p2.fX - p1.fX);
    Float b1 = ((p1.fX * p2.fY) - p2.fX*p1.fY) / (p1.fX - p2.fX);

    Float a2 = (p4.fY - p3.fY)/(p4.fX - p3.fX);
    Float b2 = ((p3.fX * p4.fY) - (p4.fX * p3.fY)) / (p3.fX - p4.fX);

    CrossP.fX = (b2 - b1) / (a1 - a2);
    CrossP.fY =a1*CrossP.fX +b1;
    return CrossP;
}

void  BookPage::CalcPoints()
{
    // 
    // �����Ͻ�Ϊ���㣬�����䵽�϶�����е�����
    //
    _middleX = (_touchPoint.fX + _cornerX)/2;
    _middleY = (_touchPoint.fY + _cornerY)/2;

    // 
    // �����м��������������ߵĿ��������
    //
    _bezierControlOne.fX = _middleX -(_cornerY - _middleY) * (_cornerY - _middleY) / (_cornerX - _middleX);
    _bezierControlOne.fY = _cornerY;

    _bezierControlTwo.fX = _cornerX;
    _bezierControlTwo.fY = _middleY - (_cornerX - _middleX) * (_cornerX - _middleX) / (_cornerY - _middleY);

    _bezierStartOne.fX = _bezierControlOne.fX - (_cornerX - _bezierControlOne.fX) / 2;
    _bezierStartOne.fY = _cornerY;

    // ��mBezierStart1.x < 0����mBezierStart1.x > 480ʱ  // 800
    // ���������ҳ�������BUG���ڴ�����
    if (_touchPoint.fX > 0 && _touchPoint.fX < _width) 
    {
        if (_bezierStartOne.fX < 0 || _bezierStartOne.fX > _width) 
        {
            if (_bezierStartOne.fX < 0)
            {
                _bezierStartOne.fX = _width - _bezierStartOne.fX;
            }

            Float f1 = abs(_cornerX - (int)_touchPoint.fX);
            Float f2 = _width * f1 / _bezierStartOne.fX;
            _touchPoint.fX = abs(_cornerX - (int)f2);

            Float f3 = abs(_cornerX - (int)_touchPoint.fX) * abs(_cornerY - (int)_touchPoint.fY) / f1;
            _touchPoint.fY = abs(_cornerY - (int)f3);

            _middleX = (_touchPoint.fX + _cornerX) / 2.0f;
            _middleY = (_touchPoint.fY + _cornerY) / 2.0f;

            _bezierControlOne.fX = _middleX - (_cornerY - _middleY) * (_cornerY - _middleY) / (_cornerX - _middleX);
            _bezierControlOne.fY = _cornerY;

            _bezierControlTwo.fX = _cornerX;
            _bezierControlTwo.fY = _middleY - (_cornerX - _middleX) * (_cornerX - _middleX) / (_cornerY - _middleY);

            _bezierStartOne.fX = _bezierControlOne.fX - (_cornerX - _bezierControlOne.fX) / 2;
        }
    }

    _bezierStartTwo.fX = _cornerX;
    _bezierStartTwo.fY = _bezierControlTwo.fY - (_cornerY - _bezierControlTwo.fY) / 2;

    _touchToCornerDis = (Float)_hypot((_touchPoint.fX - _cornerX),(_touchPoint.fY - _cornerY));

    _bezierEndOne = GetCross(_touchPoint, _bezierControlOne, _bezierStartOne,_bezierStartTwo);
    _bezierEndTwo = GetCross(_touchPoint, _bezierControlTwo, _bezierStartOne,_bezierStartTwo);

    /*
    * mBeziervertex1.x �Ƶ�
    * ((mBezierStart1.x+mBezierEnd1.x)/2+mBezierControl1.x)/2 ����ȼ���
    * (mBezierStart1.x+ 2*mBezierControl1.x+mBezierEnd1.x) / 4
    */
    _bezierVertexOne.fX = (_bezierStartOne.fX + 2 * _bezierControlOne.fX + _bezierEndOne.fX) / 4;
    _bezierVertexOne.fY = (2 * _bezierControlOne.fY + _bezierStartOne.fY + _bezierEndOne.fY) / 4;
    _bezierVertexTwo.fX = (_bezierStartTwo.fX + 2 * _bezierControlTwo.fX + _bezierEndTwo.fX) / 4;
    _bezierVertexTwo.fY = (2 * _bezierControlTwo.fY + _bezierStartTwo.fY + _bezierEndTwo.fY) / 4;
}

void BookPage::DrawCurrentPageArea(SkCanvas *canvas, SkBitmap *bitmap)
{
    mPath0.reset();
    mPath0.moveTo(_bezierStartOne.fX, _bezierStartOne.fY);
    mPath0.quadTo(_bezierControlOne.fX, _bezierControlOne.fY, _bezierEndOne.fX,_bezierEndOne.fY);
    mPath0.lineTo(_touchPoint.fX, _touchPoint.fY);
    mPath0.lineTo(_bezierEndTwo.fX, _bezierEndTwo.fY);
    mPath0.quadTo(_bezierControlTwo.fX, _bezierControlTwo.fY, _bezierStartTwo.fX,_bezierStartTwo.fY);
    mPath0.lineTo(_cornerX, _cornerY);
    mPath0.close();

    canvas->save();
    canvas->clipPath(mPath0, SkRegion::Op::kXOR_Op);
    SkPaint paint;
    paint.setAntiAlias(true);   // ����Ϊ�޾��
    canvas->drawBitmap(*bitmap, 0, 0, &paint);
    canvas->restore();
}

void BookPage::DrawNextPageAreaAndShadow(SkCanvas *canvas, SkBitmap *bitmap)
{
    mPath1.reset();
    mPath1.moveTo(_bezierStartOne.fX, _bezierStartOne.fY);
    mPath1.lineTo(_bezierVertexOne.fX, _bezierVertexOne.fY);
    mPath1.lineTo(_bezierVertexTwo.fX, _bezierVertexTwo.fY);
    mPath1.lineTo(_bezierStartTwo.fX, _bezierStartTwo.fY);
    mPath1.lineTo(_cornerX, _cornerY);
    mPath1.close();

    _degrees = (Float)(atan2(_bezierControlOne.fX - _cornerX, _bezierControlTwo.fY - _cornerY))*180/PI;

    int leftx;
    int rightx;
    SkShader *mBackShadowDrawable;

    if (mIsRTandLB)
    {
        leftx = (int)(_bezierStartOne.fX);
        rightx = (int)(_bezierStartOne.fX + _touchToCornerDis / 4);
        mBackShadowDrawable = mBackShadowDrawableLR;  // ��������ʾ��Ӱ
    }
    else
    {
        leftx = (int)(_bezierStartOne.fX - _touchToCornerDis / 4);
        rightx = (int)_bezierStartOne.fX;
        mBackShadowDrawable = mBackShadowDrawableRL;  // ���ҵ�����ʾ��Ӱ
    }

    mBackShadowDrawable = mFolderShadowDrawableRL;
    canvas->save();
    canvas->clipPath(mPath0);
    canvas->clipPath(mPath1, SkRegion::Op::kIntersect_Op);
    canvas->drawBitmap(*bitmap, 0, 0, NULL);

    canvas->translate(_bezierStartOne.fX, _bezierStartOne.fY);
    canvas->rotate(_degrees);
    canvas->translate(-_bezierStartOne.fX, -_bezierStartOne.fY);

    //mBackShadowDrawable.setBounds(leftx, (int) mBezierStart1.fY, rightx,(int)(mMaxLength + mBezierStart1.fY));
    //mBackShadowDrawable.draw(canvas);
    // Skiaû��ʵ�֣�
    SkPaint paint;

    // ����Ϊ�޾��
    paint.setAntiAlias(true);   
    paint.setShader(mBackShadowDrawable);
    canvas->drawRectCoords(leftx, (int) _bezierStartOne.fY, rightx,(int)(_maxLength + _bezierStartOne.fY), paint);

    canvas->restore();
}

void BookPage::SetBitmaps(SkBitmap *bm1, SkBitmap *bm2)
{
    _curPage = bm1;
    _nextPage = bm2;
}

void BookPage::SetScreen(int w, int h) 
{
    _width = w;
    _height = h;
}

//@Override
//protected void onDraw(Canvas canvas) 
//{
//  canvas.drawColor(0xFFAAAAAA);
//  calcPoints();
//  drawCurrentPageArea(canvas, mCurPageBitmap, mPath0); ��ǰҳ����
//  drawNextPageAreaAndShadow(canvas, mNextPageBitmap);  ��һҳ�������Ӱ
//  drawCurrentPageShadow(canvas);                       ��ǰҳ��Ӱ
//  drawCurrentBackArea(canvas, mCurPageBitmap);         ��ǰҳ��Ӱ
//}


//��java��computeScroll�����иı�mTouch,������Ϊ����
void BookPage::DrawPage(SkCanvas *canvas, SkPoint touch)
{
    //canvas->drawColor(0xFF00AAAA);
    _touchPoint = touch;

    //CalcCornerXY(mTouch.fX,mTouch.fY);//���ڵ��ô���һ���ƶ�ֻ�ܵ���һ��
    CalcPoints();
    DrawCurrentPageArea(canvas,_curPage);
    DrawNextPageAreaAndShadow(canvas,_nextPage);

    //DrawCurrentPageShadow(canvas);
    //DrawCurrentBackArea(canvas,mCurPageBitmap);
}

//  ���Ʒ���ҳ����Ӱ
void BookPage::DrawCurrentPageShadow(SkCanvas *canvas) 
{
    Float degree;

    if (mIsRTandLB)
    {
        degree = PI / 4 - atan2(_bezierControlOne.fY - _touchPoint.fY, _touchPoint.fX - _bezierControlOne.fX);
    }
    else
    {
        degree = PI / 4 - atan2(_touchPoint.fY - _bezierControlOne.fY, _touchPoint.fX - _bezierControlOne.fX);
    }

    // ����ҳ��Ӱ������touch��ľ���
    Float d1 = (Float) 25 * 1.414 * cos(degree);
    Float d2 = (Float) 25 * 1.414 * sin(degree);
    Float x = (Float) (_touchPoint.fX + d1);
    Float y;

    if (mIsRTandLB)
    {
        y = (Float) (_touchPoint.fY + d2);
    } 
    else
    {
        y = (Float) (_touchPoint.fY - d2);
    }

    mPath1.reset();
    mPath1.moveTo(x, y);
    mPath1.lineTo(_touchPoint.fX, _touchPoint.fY);
    mPath1.lineTo(_bezierControlOne.fX, _bezierControlOne.fY);
    mPath1.lineTo(_bezierStartOne.fX, _bezierStartOne.fY);
    mPath1.close();
    Float rotateDegrees;
    canvas->save();
    canvas->clipPath(mPath0, SkRegion::Op::kXOR_Op);
    canvas->clipPath(mPath1, SkRegion::Op::kIntersect_Op);
    int leftx;
    int rightx;
    SkShader *mCurrentPageShadow;

    if (mIsRTandLB) 
    {
        leftx = (int) (_bezierControlOne.fX);
        rightx = (int) _bezierControlOne.fX + 25;
        //mCurrentPageShadow = mFrontShadowDrawableVLR;
    } 
    else
    {
        leftx = (int) (_bezierControlOne.fX - 25);
        rightx = (int) _bezierControlOne.fX + 1;
        //mCurrentPageShadow = mFrontShadowDrawableVRL;
    }

    mCurrentPageShadow = mFolderShadowDrawableRL;

    //*********************************************************��Ҫ�ص㳢��,
    //����ʹ��LinearGradient;
    //SkPaint paint;
    //paint.setShader(SkShader *shader);
    //SkShader linearShader = SkGradientShader::CreateLinear();
    //paint.setShader(linearShader);
    //�м������ת�Լ�·������

    //mPaint.setShader(mComposeShader);  
    //canvas.drawRect(0, 0, imgwidth, imgheight, mPaint);  
    //drawRect(rect��Χ������mCurrentPageShadow.SetBounds�Ĳ���ֵ)
    //canvas->drawRoundRect()

    //����ԭ������Ӱ����GradientDrawableʵ��
    //��android��Դ���У�GradientDrawable�ľ���ʵ���ǣ�graphics/drawable/GradientDrawable.java
    //ensureValidRect()�����У����GradiemtDrawable������LINEAR_GRADIENT,
    //GradientDrawable�ĳ�ԱmFillPaint:
    //mFillPaint.setShader(new LinearGradient(x0, y0, x1, y1,colors, st.mPositions, Shader.TileMode.CLAMP));
    //ensureValidRect()��GradientDrawable��draw(Canvas canvas)��ʼ������

    //**********************************************************8

    //rotateDegrees = (Float) Math.toDegrees(atan2(mTouch.x - mBezierControl1.x, mBezierControl1.y - mTouch.y));
    rotateDegrees = (Float)(atan2(_touchPoint.fX - _bezierControlOne.fX, _bezierControlOne.fY - _touchPoint.fY))*180/PI;

    //canvas.rotate(rotateDegrees, mBezierControl1.x, mBezierControl1.y);
    canvas->translate(_bezierControlOne.fX, _bezierControlOne.fY);
    canvas->rotate(rotateDegrees);
    canvas->translate(-_bezierControlOne.fX, -_bezierControlOne.fY);

    SkPaint paint;
    paint.setShader(mCurrentPageShadow);
    canvas->drawRectCoords(leftx,(int)(_bezierControlOne.fY - _maxLength), rightx,(int)(_bezierControlOne.fY),paint);
    //mCurrentPageShadow.setBounds(leftx,(int)(mBezierControl1.y - mMaxLength), rightx,(int)(mBezierControl1.y));
    //mCurrentPageShadow.draw(canvas);

    canvas->restore();

    mPath1.reset();
    mPath1.moveTo(x, y);
    mPath1.lineTo(_touchPoint.fX, _touchPoint.fY);
    mPath1.lineTo(_bezierControlTwo.fX, _bezierControlTwo.fY);
    mPath1.lineTo(_bezierStartTwo.fX, _bezierStartTwo.fY);
    mPath1.close();
    canvas->save();
    canvas->clipPath(mPath0, SkRegion::Op::kXOR_Op);
    canvas->clipPath(mPath1, SkRegion::Op::kIntersect_Op);

    if (mIsRTandLB)
    {
        leftx = (int) (_bezierControlTwo.fY);
        rightx = (int) (_bezierControlTwo.fY + 25);
        //mCurrentPageShadow = mFrontShadowDrawableHTB;
    } 
    else 
    {
        leftx = (int) (_bezierControlTwo.fY - 25);
        rightx = (int) (_bezierControlTwo.fY + 1);
        //mCurrentPageShadow = mFrontShadowDrawableHBT;
    }

    //rotateDegrees = (Float) Math.toDegrees(Math.atan2(mBezierControl2.y - mTouch.y, mBezierControl2.x - mTouch.x));
    rotateDegrees = (Float)(atan2(_bezierControlTwo.fY - _touchPoint.fY, _bezierControlTwo.fX - _touchPoint.fX))*180/PI;
    //canvas.rotate(rotateDegrees, mBezierControl2.x, mBezierControl2.y);
    canvas->translate(_bezierControlTwo.fX, _bezierControlTwo.fY);
    canvas->rotate(rotateDegrees);
    canvas->translate(-_bezierControlTwo.fX, -_bezierControlTwo.fY);
    Float temp;

    if (_bezierControlTwo.fY < 0)
    {
        temp = _bezierControlTwo.fY - _height;
    }
    else
    {
        temp = _bezierControlTwo.fY;
    }

    int hmg = (int) _hypot(_bezierControlTwo.fX, temp);
    //if (hmg > mMaxLength)
    //  mCurrentPageShadow.setBounds((int) (mBezierControl2.x - 25) - hmg, leftx,
    //                      (int)(mBezierControl2.x + mMaxLength) - hmg,rightx);
    //else
    //  mCurrentPageShadow.setBounds((int) (mBezierControl2.x - mMaxLength), leftx,
    //                      (int) (mBezierControl2.x), rightx);

    // Log.i("hmg", "mBezierControl2.x   " + mBezierControl2.x
    // + "  mBezierControl2.y  " + mBezierControl2.y);
    //mCurrentPageShadow.draw(canvas);
    if (hmg > _maxLength)
    {
        canvas->drawRectCoords((int)(_bezierControlTwo.fX - 25) - hmg, leftx,(int)(_bezierControlTwo.fX + _maxLength) - hmg,rightx,paint);
    }
    else
    {
        canvas->drawRectCoords((int)(_bezierControlTwo.fX - _maxLength),leftx,(int)(_bezierControlTwo.fX), rightx,paint);
    }

    canvas->restore();
}

//���Ʒ���ҳ����
void BookPage::DrawCurrentBackArea(SkCanvas *canvas, SkBitmap *bitmap) 
{
    int i = (int) (_bezierStartOne.fX + _bezierControlOne.fX) / 2;
    Float f1 = Math::Abs(i - _bezierControlOne.fX);
    int i1 = (int) (_bezierStartTwo.fY + _bezierControlTwo.fY) / 2;
    Float f2 = Math::Abs(i1 - _bezierControlTwo.fY);
    Float f3 = min(f1, f2);
    mPath1.reset();
    mPath1.moveTo(_bezierVertexTwo.fX, _bezierVertexTwo.fY);
    mPath1.lineTo(_bezierVertexOne.fX, _bezierVertexOne.fY);
    mPath1.lineTo(_bezierEndOne.fX, _bezierEndOne.fY);
    mPath1.lineTo(_touchPoint.fX, _touchPoint.fY);
    mPath1.lineTo(_bezierEndTwo.fX, _bezierEndTwo.fY);
    mPath1.close();
    SkShader *mFolderShadowDrawable;
    int left;
    int right;

    if (mIsRTandLB)
    {
        left = (int) (_bezierStartOne.fX - 1);
        right = (int) (_bezierStartOne.fX + f3 + 1);
        //mFolderShadowDrawable = mFolderShadowDrawableLR;
    } 
    else
    {
        left = (int) (_bezierStartOne.fX - f3 - 1);
        right = (int) (_bezierStartOne.fX + 1);
        //mFolderShadowDrawable = mFolderShadowDrawableRL;
    }

    mFolderShadowDrawable =mFolderShadowDrawableRL;
    canvas->save();
    canvas->clipPath(mPath0);
    canvas->clipPath(mPath1, SkRegion::Op::kIntersect_Op);

    _paint.setColorFilter(_colorMatrixFilter);

    Float dis = (Float)_hypot(_cornerX - _bezierControlOne.fX,_bezierControlTwo.fY - _cornerY);
    Float f8 = (_cornerX - _bezierControlOne.fX) / dis;
    Float f9 = (_bezierControlTwo.fY - _cornerY) / dis;
    _matrixArray[0] = 1 - 2 * f9 * f9;
    _matrixArray[1] = 2 * f8 * f9;
    _matrixArray[3] = _matrixArray[1];
    _matrixArray[4] = 1 - 2 * f8 * f8;
    _matrix.reset();

    //mMatrix.setValues(mMatrixArray);
    //mMatrix.set(mMatrixArray);

    _matrix.preTranslate(-_bezierControlOne.fX, -_bezierControlOne.fY);
    _matrix.postTranslate(_bezierControlOne.fX, _bezierControlOne.fY);
    //canvas.drawBitmap(bitmap, mMatrix, mPaint);
    canvas->drawBitmapMatrix(*bitmap,_matrix,&_paint);
    _paint.setColorFilter(NULL);

    //canvas.rotate(mDegrees, mBezierStart1.fX, mBezierStart1.fY);
    canvas->translate(_bezierStartOne.fX, _bezierStartOne.fY);
    canvas->rotate(_degrees);
    canvas->translate(-_bezierStartOne.fX, -_bezierStartOne.fY);

    SkPaint paint;
    paint.setShader(mFolderShadowDrawable);
    canvas->drawRectCoords(left, (int) _bezierStartOne.fY, right,(int) (_bezierStartOne.fY + _maxLength),paint);
    //mFolderShadowDrawable.setBounds(left, (int) mBezierStart1.fY, right,
    //      (int) (mBezierStart1.fY + mMaxLength));
    //mFolderShadowDrawable.draw(canvas);
    canvas->restore();
}

//public void computeScroll() {
//  super.computeScroll();
//  if (mScroller.computeScrollOffset()) {
//      Float x = mScroller.getCurrX();
//      Float y = mScroller.getCurrY();
//      mTouch.x = x;
//      mTouch.y = y;
//      postInvalidate();
//  }
//}

void BookPage::StartAnimation(int delayMillis)
{
    int dx, dy;

    // dx ˮƽ���򻬶��ľ��룬��ֵ��ʹ�����������
    // dy ��ֱ���򻬶��ľ��룬��ֵ��ʹ�������Ϲ���
    if (_cornerX > 0) 
    {
        dx = -(int)(_width + _touchPoint.fX);
    } 
    else
    {
        dx = (int)(_width - _touchPoint.fX + _width);
    }
    if (_cornerY > 0)
    {
        dy = (int) (_height - _touchPoint.fY);
    } 
    else
    {
        dy = (int) (1 - _touchPoint.fY); // ��ֹmTouch.y���ձ�Ϊ0
    }
    //mScroller.startScroll((int) mTouch.x, (int) mTouch.y, dx, dy,delayMillis);
}

//�˳�animation
void BookPage::AbortAnimation()
{
    //if (!mScroller.isFinished()) 
    //{
    //  mScroller.abortAnimation();
    //}
}

BOOL BookPage::CanDragOver()
{
    if (_touchToCornerDis > _width / 10)
    {
        return TRUE;
    }

    return FALSE;
}

// �Ƿ����߷����ұ�
BOOL BookPage::DragToRight()
{
    if (_cornerX > 0)
    {
        return FALSE;
    }

    return TRUE;
}
BOOL BookPage::LButtonDown(SkCanvas *canvas,SkPoint pt)
{
    CalcCornerXY(pt.fX, pt.fY);
    DrawPage(canvas, pt);
    return TRUE;
}

BOOL BookPage::MouseMove(SkCanvas *canvas, SkPoint pt)
{
    DrawPage(canvas, pt);
    return TRUE;
}

BOOL BookPage::LButtonUp(SkCanvas *canvas,SkPoint pt)
{
    DrawPage(canvas,pt);
    //canvas->drawBitmap(*mNextPageBitmap,0,0,NULL);
    return TRUE;
}
