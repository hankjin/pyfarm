#include "skin.h"

HRGN RegionFromSkin(HBITMAP hBmp, COLORREF crMask){
	
	HRGN		hRgn = CreateRectRgn(0, 0, 0, 0);
	int			x = 0;
	BITMAP		bmp;
	HDC			hdcMem;
	HBITMAP		hbOld;

	hdcMem = CreateCompatibleDC(GetDC(0));
	hbOld = SelectObject(hdcMem, hBmp);
	GetObject(hBmp, sizeof(bmp), &bmp);
	for(int y = 0; y < bmp.bmHeight; y++){
		do{
			while(x < bmp.bmWidth && GetPixel(hdcMem, x, y) == crMask)
				x++;
			int leftX = x;
			while(x < bmp.bmWidth && GetPixel(hdcMem, x, y) != crMask)
				++x;
			HRGN rgnTemp = CreateRectRgn(leftX, y, x, y + 1);
			CombineRgn(hRgn, hRgn, rgnTemp, RGN_OR);
			DeleteObject(rgnTemp);
		}while(x < bmp.bmWidth);
		x = 0;
	}
	SelectObject(hdcMem, hbOld);
	DeleteDC(hdcMem);
	return hRgn;
}

HRGN RegionFromSkinWithOffset(HBITMAP hBmp, COLORREF crMask, int offset, int bWidth){
	
	HRGN		hRgn = CreateRectRgn(0, 0, 0, 0);
	int			x = offset;
	BITMAP		bmp;
	HDC			hdcMem;
	HBITMAP		hbOld;

	bWidth += offset;
	hdcMem = CreateCompatibleDC(GetDC(0));
	hbOld = SelectObject(hdcMem, hBmp);
	GetObject(hBmp, sizeof(bmp), &bmp);
	for(int y = 0; y < bmp.bmHeight; y++){
		do{
			while(x < bWidth && GetPixel(hdcMem, x, y) == crMask)
				x++;
			int leftX = x;
			while(x < bWidth && GetPixel(hdcMem, x, y) != crMask)
				++x;
			HRGN rgnTemp = CreateRectRgn(leftX, y, x, y + 1);
			CombineRgn(hRgn, hRgn, rgnTemp, RGN_OR);
			DeleteObject(rgnTemp);
		}while(x < bWidth);
		x = offset;
	}
	SelectObject(hdcMem, hbOld);
	DeleteDC(hdcMem);
	return hRgn;
}
