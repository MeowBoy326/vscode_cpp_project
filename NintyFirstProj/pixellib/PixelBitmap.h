#ifndef __PIXEL_BITMAP_H__
#define __PIXEL_BITMAP_H__

#include "ibitmap.h"
#include "ibmcols.h"
#include "ibmdata.h"
#include "ibmwink.h"

#include <stddef.h>
#include <string.h>

namespace Pixel 
{

// ��ɫ��ʽ
enum Format
{
	// 32 bits
	A8R8G8B8 = 0,
	A8B8G8R8 = 1,
	R8G8B8A8 = 2,
	B8G8R8A8 = 3,
	X8R8G8B8 = 4,
	X8B8G8R8 = 5,
	R8G8B8X8 = 6,
	B8G8R8X8 = 7,
	P8R8G8B8 = 8,
	
	// 24 bits
	R8G8B8 = 9,
	B8G8R8 = 10,
	
	// 16 bits
	R5G6B5 = 11,
	B5G6R5 = 12,
	X1R5G5B5 = 13,
	X1B5G5R5 = 14,
	R5G5B5X1 = 15,
	B5G5R5X1 = 16,
	A1R5G5B5 = 17,
	A1B5G5R5 = 18,
	R5G5B5A1 = 19,
	B5G5R5A1 = 20,
	X4R4G4B4 = 21,
	X4B4G4R4 = 22,
	R4G4B4X4 = 23,
	B4G4R4X4 = 24,
	A4R4G4B4 = 25,
	A4B4G4R4 = 26,
	R4G4B4A4 = 27,
	B4G4R4A4 = 28,
	
	// 8 bits
	C8 = 29,
	G8 = 30,
	A8 = 31,
	R3G3B2 = 32,
	B2G3R3 = 33,
	X2R2G2B2 = 34,
	X2B2G2R2 = 35,
	R2G2B2X2 = 36,
	B2G2R2X2 = 37,
	A2R2G2B2 = 38,
	A2B2G2R2 = 39,
	R2G2B2A2 = 40,
	B2G2R2A2 = 41,
	X4C4 = 42,
	X4G4 = 43,
	X4A4 = 44,
	C4X4 = 45,
	G4X4 = 46,
	A4X4 = 47,
};

// ����������
enum Filter
{
	BILINEAR = 0,
	NEAREST = 1,
};

// Խ�����
enum OverflowMode
{
	OM_TRANSPARENT = 0,		// Խ��ʱ��ʹ��͸��ɫ���� IBITMAP::mask
	OM_REPEAT = 1,			// Խ��ʱ���ظ��߽���ɫ
	OM_WRAP = 2,			// Խ��ʱ��ʹ����һ����ɫ
	OM_MIRROR = 3,			// Խ��ʱ��ʹ�þ�����ɫ
};

// �����
enum AntiAliasing
{
	AA_HIGH	= 0,		// �����������
	AA_LOW	= 1,		// �����������
	AA_OFF	= 2,		// �رտ����
};

// ��Ķ���
struct Point
{
	double x;
	double y;
};

// ��������
struct PixelError {
	PixelError(const char *what) {
		size_t size = strlen(what);
		_what = new char[size + 1];
		for (size_t i = 0; i < size; i++) _what[i] = what[i];
		_what[size] = 0;
	}
	virtual ~PixelError() { if (_what) delete _what; }
	const char *what() { return _what; }
	char *_what;
};

// λͼ����
struct BitmapError : public PixelError { 
	BitmapError(const char *what) : PixelError(what) {  } 
};


// ͼ����º�����
// ���أ�����0����ɹ���1����д�أ�С����Ϊ����
typedef int (*UpdateProc)(int x, int y, int w, IUINT32 *card, void *user);

// ��ɫ���
struct ColorMatrix
{
	float m[4][5];
};


// ��ӿ�
class Bitmap;			// λͼ��
class Gradient;			// �ݶ�
class Source;			// ͼ��Դ
class Paint;			// ��ͼ����


//=====================================================================
// BITMAP ����
//=====================================================================
class Bitmap
{
public:
	virtual ~Bitmap();
	Bitmap();
	Bitmap(int width, int height, enum Format pixfmt = A8R8G8B8);
	Bitmap(const char *filename, IRGB *pal = NULL);
	Bitmap(int width, int height, enum Format pixfmt, long pitch, void *bits);
	Bitmap(IBITMAP *bmp, bool ownbits = false);
	Bitmap(const void *picmem, size_t size, IRGB *pal = NULL);

	//////////////////////// �����ӿ� ////////////////////////

	// ����
	virtual int Create(int width, int height, enum Format pixfmt = A8R8G8B8);

	// �����е��ڴ�鴴��
	virtual int Create(int width, int height, enum Format pixfmt, long pitch, void *bits);

	// ɾ��
	virtual void Release();

	// ���ļ���ȡ
	virtual int Load(const char *filename, IRGB *pal = NULL);

	// ���ڴ��ȡ
	virtual int Load(const void *picmem, size_t size, IRGB *pal = NULL);

	// ���ⲿ����һ��IBITMAP
	int Assign(IBITMAP *bmp, bool ownbits = false);

	int GetW() const;				// ȡ�ÿ�
	int GetH() const;				// ȡ�ø�
	int GetBPP() const;				// ȡ��ɫ��
	long GetPitch() const;			// ȡ��Row Stride
	enum Format GetFormat() const;	// ȡ�����ظ�ʽ

	IBITMAP *GetBitmap();		// ȡ�� IBITMAP
	const IBITMAP *GetBitmap() const;	// ȡ�� IBITMAP
	IUINT8 *GetLine(int line);	// ȡ��ɨ����
	IUINT8 *GetPixel();			// ȡ�����ص�

	// ���û��Ʋü�����
	void SetClip(const IRECT *rect);	
	void SetClip(int left, int top, int right, int bottom); 

	void SetMask(IUINT32 mask);						// ���ùؼ�ɫ
	void SetFilter(enum Filter filter);				// ���ò�������
	void SetOverflow(enum OverflowMode mode);		// ����Խ��ģʽ
	void SetAntiAliasing(enum AntiAliasing mode);	// ���ÿ����


	//////////////////////// �򵥻��� ////////////////////////
	virtual int Blit(int x, int y, const IBITMAP *src, const IRECT *rect = NULL, int mode = 0);
	virtual int Blit(int x, int y, const IBITMAP *src, int sx, int sy, int sw, int sh, int mode = 0);
	virtual int Blit(int x, int y, const Bitmap *src, const IRECT *rect = NULL, int mode = 0);
	virtual int Blit(int x, int y, const Bitmap *src, int sx, int sy, int sw, int sh, int mode = 0);

	virtual int Stretch(const IRECT *bound_dst, const IBITMAP *src, const IRECT *bound_src, int mode = 0);
	virtual int Stretch(const IRECT *bound_dst, const Bitmap *src, const IRECT *bound_src, int mode = 0);

	virtual void FillRaw(IUINT32 rawcolor, const IRECT *rect = NULL);
	virtual void FillRaw(IUINT32 rawcolor, int x, int y, int w, int h);

	//////////////////////// ��ɫ���� ////////////////////////	
	virtual int Blend(int x, int y, const IBITMAP *src, const IRECT *rect = NULL, IUINT32 color = 0xffffffff, int mode = 0);
	virtual int Blend(int x, int y, const IBITMAP *src, int sx, int sy, int sw, int sh, IUINT32 color = 0xffffffff, int mode = 0);
	virtual int Blend(int x, int y, const Bitmap *src, const IRECT *rect = NULL, IUINT32 color = 0xffffffff, int mode = 0);
	virtual int Blend(int x, int y, const Bitmap *src, int sx, int sy, int sw, int sh, IUINT32 color = 0xffffffff, int mode = 0);

	virtual void Fill(IUINT32 rgba, const IRECT *rect = NULL);
	virtual void Fill(IUINT32 rgba, int x, int y, int w, int h);

	virtual int MaskFill(int x, int y, const IBITMAP *src, const IRECT *rect = NULL, IUINT32 color = 0xffffffff);
	virtual int MaskFill(int x, int y, const IBITMAP *src, int sx, int sy, int sw, int sh, IUINT32 color);
	virtual int MaskFill(int x, int y, const Bitmap *src, const IRECT *rect = NULL, IUINT32 color = 0xffffffff);
	virtual int MaskFill(int x, int y, const Bitmap *src, int sx, int sy, int sw, int sh, IUINT32 color);

	//////////////////////// ���դ�� ////////////////////////	
	virtual int Raster(const Point *pts, const IBITMAP *src, const IRECT *bound = NULL, IUINT32 color = 0xffffffff, int mode = 0);
	virtual int Raster(const Point *pts, const Bitmap *src, const IRECT *bound = NULL, IUINT32 color = 0xffffffff, int mode = 0);

	//////////////////////// �߼����� ////////////////////////	
	virtual int Draw2D(	double x, double y, const IBITMAP *src, const IRECT *bound = NULL, 
						double OffsetX = 0.0, double OffsetY = 0.0, double ScaleX = 1.0, double ScaleY = 1.0, 
						double Angle = 0.0, IUINT32 color = 0xffffffff);
	virtual int Draw2D( double x, double y, const Bitmap *src, const IRECT *bound = NULL, 
						double OffsetX = 0.0, double OffsetY = 0.0, double ScaleX = 1.0, double ScaleY = 1.0, 
						double Angle = 0.0, IUINT32 color = 0xffffffff);

	//////////////////////// ͸�ӻ��� ////////////////////////	
	virtual int Draw3D( double x, double y, double z, const IBITMAP *src, const IRECT *bound = NULL,
						double OffsetX = 0.0, double OffsetY = 0.0, double ScaleX = 1.0, double ScaleY = 1.0,
						double AngleX = 0.0, double AngleY = 0.0, double AngleZ = 0.0, IUINT32 color = 0xffffffff);
	virtual int Draw3D( double x, double y, double z, const Bitmap *src, const IRECT *bound = NULL,
						double OffsetX = 0.0, double OffsetY = 0.0, double ScaleX = 1.0, double ScaleY = 1.0,
						double AngleX = 0.0, double AngleY = 0.0, double AngleZ = 0.0, IUINT32 color = 0xffffffff);

	//////////////////////// ͼ��ϳ� ////////////////////////	
	int Composite(int x, int y, const IBITMAP *src, int sx, int sy, int sw, int sh, int op, int flags = 0);
	int Composite(int x, int y, const Bitmap *src, int sx, int sy, int sw, int sh, int op, int flags = 0);
	int Composite(int x, int y, const IBITMAP *src, int op, const IRECT *rect = NULL, int flags = 0);
	int Composite(int x, int y, const Bitmap *src, int op, const IRECT *rect = NULL, int flags = 0);

	//////////////////////// λͼ���� ////////////////////////	
	Bitmap *Convert(enum Format NewFormat, const IRGB *spal = NULL, const IRGB *dpal = NULL);
	Bitmap *Resample(int NewWidth, int NewHeight, const IRECT *bound = NULL);
	Bitmap *Chop(const IRECT *bound);
	Bitmap *Chop(int left, int top, int right, int bottom);
	int ConvertSelf(enum Format NewFormat);

	//////////////////////// ɫ�ʱ�� ////////////////////////	
	void ColorTransform(const ColorMatrix *t, const IRECT *bound = NULL);
	void ColorAdd(IUINT32 color, const IRECT *bound = NULL);
	void ColorSub(IUINT32 color, const IRECT *bound = NULL);
	void ColorMul(IUINT32 color, const IRECT *bound = NULL);

	int ColorUpdate(UpdateProc updater, bool readonly, void *user = NULL, const IRECT *bound = NULL);
	void Blur(int rx, int ry, const IRECT *bound = NULL);
	void Gray(const IRECT *bound = NULL);
	void AdjustHSV(float Hue, float Saturation = 1.0, float Value = 1.0);
	void AdjustHSL(float Hue, float Saturation = 1.0, float Lightness = 1.0);

	//////////////////////// ͼ����Ч ////////////////////////	
	Bitmap* DropShadow(int radius_x = 3, int radius_y = 3);
	Bitmap* RoundCorner(int radius = 6, int style = 0);
	Bitmap* GlossyMake(int radius, int border, int light, int shadow, int shadow_pos = 0);
	Bitmap* MiiMake(int lightmode = 1, bool roundcorner = true, bool border = false, bool shadow = false);

	void InitColorIndex(iColorIndex *index, const IRGB *pal, int palsize = 256);

	//////////////////////// ��׿��ͼ ////////////////////////	
	Bitmap* AndroidPatch9(int NewWidth, int NewHeight, int *errcode = NULL); // ����patch�����´�СͼƬ
	int AndroidClient(IRECT *client = NULL);	// ȡ�ÿͻ��˴�С

protected:
	static int UpdateGray(int x, int y, int w, IUINT32 *card, void *user);

protected:
	IBITMAP *bitmap;
	IRECT clip;
	int pixfmt;
	int classcode;
	bool reference;
	bool ownbits;
};



//=====================================================================
// ɫ���ݶ�
//=====================================================================

// �ݶȽڵ�
struct GradientStop
{
	double x;
	IUINT32 color;
};

// ɫ���ݶ�
class Gradient
{
public:
	virtual ~Gradient();
	Gradient();
	Gradient(const GradientStop *stops, int nstops);

	void Init(const GradientStop *stops, int nstops);
	void AddStop(double x, IUINT32 color);
	void AddStop(const GradientStop *stop);
	void Reset();

	ipixel_gradient_t *GetGradient();
	ipixel_gradient_stop_t *GetStops();
	int GetLength();

	void SetOverflow(enum OverflowMode mode = OM_REPEAT);

protected:
	int resize_stops(int newsize, bool force = false);
	void initialize();
	void destroy();
	int update();

protected:
	cvector_t float_stops;
	cvector_t fixed_stops;
	int nstops;
	bool dirty;
	GradientStop *fstops;
	ipixel_gradient_stop_t *stops;
	ipixel_gradient_t gradient;
};


//=====================================================================
// ����Դ
//=====================================================================
class Source
{
public:
	Source();
	Source(IUINT32 color);
	Source(IBITMAP *bitmap);
	Source(Bitmap *bitmap);

	//////////////////////// ��ʼ���� ////////////////////////	
	void InitSolid(IUINT32 color = 0xffffffff);

	void InitBitmap(IBITMAP *bitmap);
	void InitBitmap(Bitmap *bitmap);

	void InitGradientLinear(ipixel_gradient_t *g, const Point *p1, const Point *p2);
	void InitGradientLinear(ipixel_gradient_t *g, double x1, double y1, double x2, double y2);
	void InitGradientLinear(Gradient *g, const Point *p1, const Point *p2);
	void InitGradientLinear(Gradient *g, double x1, double y1, double x2, double y2);

	void InitGradientRadial(ipixel_gradient_t *g, const Point *inner, const Point *outer, 
		double inner_radius, double outer_radius);
	void InitGradientRadial(ipixel_gradient_t *g, double inner_x, double inner_y, 
		double outer_x, double outer_y, double inner_r, double outer_r);

	void InitGradientRadial(Gradient *g, const Point *inner, const Point *outer, 
		double inner_radius, double outer_radius);
	void InitGradientRadial(Gradient *g, double inner_x, double inner_y, 
		double outer_x, double outer_y, double inner_r, double outer_r);

	void InitGradientConical(ipixel_gradient_t *g, const Point *center, double angle);
	void InitGradientConical(ipixel_gradient_t *g, double cx, double cy, double angle);
	void InitGradientConical(Gradient *g, const Point *center, double angle);
	void InitGradientConical(Gradient *g, double cx, double cy, double angle);

	//////////////////////// ���ù��� ////////////////////////	
	void SetTransform(const ipixel_transform_t *t);
	void SetOverflow(enum OverflowMode mode = OM_REPEAT, IUINT32 transparent = 0);
	void SetFilter(enum Filter filter = BILINEAR);
	void SetBound(const IRECT *bound = NULL);

	//////////////////////// �ؼ��ӿ� ////////////////////////	
	int FetchScanline(int x, int y, int width, IUINT32 *card, const IUINT8 *mask = NULL);

public:
	ipixel_source_t source;
};


//=====================================================================
// ��ͼ����
//=====================================================================
class Paint
{
public:
	virtual ~Paint();
	Paint(IBITMAP *bitmap);
	Paint(Bitmap *bitmap);

public:
	void Create(IBITMAP *bitmap);
	void Create(Bitmap *bitmap);
	void Release();

	void SetBitmap(IBITMAP *bitmap);
	void SetBitmap(Bitmap *bitmap);

	ipaint_t *GetPaint();
	const ipaint_t *GetPaint() const;
	int GetW() const;
	int GetH() const;

	//////////////////////// ����ͼ�� ////////////////////////	
	void SetSource(ipixel_source_t *source);
	void SetSource(Source *source);
	void SetColor(IUINT32 color = 0xff000000);

	//////////////////////// �������� ////////////////////////	
	void SetClip(const IRECT *clip = NULL);
	void SetTextColor(IUINT32 color = 0xff000000);
	void SetTextBackground(IUINT32 color = 0xffffffff);
	void SetAntiAlising(int level = 2);
	void SetLineWidth(double width = 1.0);
	void SetAdditive(bool additive = false);
	void Fill(const IRECT *bound = NULL, IUINT32 color = 0);
	void Fill(int left, int top, int right, int bottom, IUINT32 color = 0);

	//////////////////////// ���Ƽ��� ////////////////////////	
	int DrawPolygon(const Point *pts, int npts);
	int DrawLine(double x1, double y1, double x2, double y2);
	int DrawCircle(double x, double y, double radius);
	int DrawEllipse(double x, double y, double rx, double ry);
	int DrawRectangle(double x1, double y1, double x2, double y2);

	//////////////////////// ����ͼ�� ////////////////////////	
	void Draw(int x, int y, const IBITMAP *bmp, const IRECT *bound = NULL, IUINT32 color = 0xffffffff, int flag = 0);
	void Draw(int x, int y, const Bitmap *bmp, const IRECT *bound = NULL, IUINT32 color = 0xffffffff, int flag = 0);
	void Raster(const ipixel_point_t *pts, const IBITMAP *bmp, const IRECT *bound = NULL, IUINT32 color = 0xffffffff, int flag = 0);
	void Raster(const ipixel_point_t *pts, const Bitmap *bmp, const IRECT *bound = NULL, IUINT32 color = 0xffffffff, int flag = 0);

	//////////////////////// ͼ��任 ////////////////////////	
	void Draw2D(double x, double y, const IBITMAP *bmp, IRECT *bound = NULL,
		double offset_x = 0.0, double offset_y = 0.0, 
		double scalex = 1.0, double scaley = 1.0, 
		double angle = 0.0, IUINT32 color = 0xffffffff);
	void Draw2D(double x, double y, const Bitmap *bmp, IRECT *bound = NULL,
		double offset_x = 0.0, double offset_y = 0.0, 
		double scalex = 1.0, double scaley = 1.0, 
		double angle = 0.0, IUINT32 color = 0xffffffff);

	void Draw3D(double x, double y, double z, const IBITMAP *bmp, IRECT *bound = NULL,
		double offset_x = 0.0, double offset_y = 0.0,
		double scalex = 1.0, double scaley = 1.0,
		double anglex = 0.0, double angley = 0.0, double anglez = 0.0, 
		IUINT32 color = 0xffffffff);
	void Draw3D(double x, double y, double z, const Bitmap *bmp, IRECT *bound = NULL,
		double offset_x = 0.0, double offset_y = 0.0,
		double scalex = 1.0, double scaley = 1.0,
		double anglex = 0.0, double angley = 0.0, double anglez = 0.0,
		IUINT32 color = 0xffffffff);

	//////////////////////// �������� ////////////////////////	
	void Printf(int x, int y, const char *fmt, ...);
	void ShadowPrintf(int x, int y, const char *fmt, ...);

protected:
	void initialize();
	void destroy();

protected:
	ipaint_t *paint;
};




};

#endif


