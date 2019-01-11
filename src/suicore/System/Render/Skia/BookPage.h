
#include <System/Types/Types.h>
#include <System/Windows/Object.h>

#include <core/SkPreConfig.h>
#include <core/SkCanvas.h>
#include <core/SkRect.h>
#include <core/SkStream.h>
#include <core/SkPoint.h>
#include <effects/SkGradientShader.h>
#include <core/SkPath.h>
#include <effects/SkColorMatrix.h>
#include <effects/SkColorMatrixFilter.h>
#include <utils/skcamera.h>

#define PI (3.141592653)

using namespace suic;

class BookPage
{
public:

    BookPage();
    ~BookPage();

    void InitBookPage(int w, int h);
    void ClearBookPage();

    void CreateShader();
    void CalcCornerXY(Float x,Float y);
    SkPoint GetCross(SkPoint p1,SkPoint p2,SkPoint p3,SkPoint p4);
    void CalcPoints();

    void DrawCurrentPageArea(SkCanvas *canvas, SkBitmap *bitmap);
    void DrawNextPageAreaAndShadow(SkCanvas *canvas, SkBitmap *bitmap);
    void SetBitmaps(SkBitmap *bm1, SkBitmap *bm2);

    void SetScreen(int w, int h);
    void DrawCurrentPageShadow(SkCanvas *canvas);
    void DrawCurrentBackArea(SkCanvas *canvas, SkBitmap *bitmap);
    void StartAnimation(int delayMillis);
    void AbortAnimation();
    BOOL CanDragOver();
    BOOL DragToRight();

    void DrawPage(SkCanvas *canvas, SkPoint touch);
    BOOL LButtonDown(SkCanvas *canvas, SkPoint pt);
    BOOL MouseMove(SkCanvas *canvas, SkPoint pt);
    BOOL LButtonUp(SkCanvas *canvas, SkPoint pt);

    int _cornerX ;  // ��ק���Ӧ��ҳ��
    int _cornerY ;

private:

    int _width;     // ��Ļ���
    int _height;    // ��Ļ�߶�

    SkPath mPath0;  //�ü�·��
    SkPath mPath1;

    SkBitmap *_curPage ;            // ��ǰҳ,��ʹ���ߴ���
    SkBitmap *_nextPage ;

    SkPoint _touchPoint;            // ��ק��
    SkPoint _bezierStartOne;        // ������������ʼ��
    SkPoint _bezierControlOne;      // ���������߿��Ƶ�
    SkPoint _bezierVertexOne;       // ���������߶���
    SkPoint _bezierEndOne;          // ���������߽�����

    SkPoint _bezierStartTwo;        // ��һ������������
    SkPoint _bezierControlTwo;
    SkPoint _bezierVertexTwo;
    SkPoint _bezierEndTwo;

    Float _middleX;
    Float _middleY;
    Float _degrees;
    Float _touchToCornerDis;
    SkColorMatrixFilter *_colorMatrixFilter;
    SkMatrix _matrix;

    bool mIsRTandLB;            // �Ƿ�������������
    Float _matrixArray[9] ;
    Float _maxLength ;          // ��Ļ�Խ���

    SkShader *mBackShadowDrawableLR;
    SkShader *mBackShadowDrawableRL;
    SkShader *mFolderShadowDrawableLR;
    SkShader *mFolderShadowDrawableRL;

    SkShader *mFrontShadowDrawableHBT;
    SkShader *mFrontShadowDrawableHTB;
    SkShader *mFrontShadowDrawableVLR;
    SkShader *mFrontShadowDrawableVRL;

    SkPaint _paint;
};
