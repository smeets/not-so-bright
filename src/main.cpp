#include <windows.h>
#include <stdio.h>
#include <chrono>
#include <stdlib.h>
#include <signal.h>
using namespace std;

bool MUST_DIE     = false;
int  REFRESH_RATE = 500;

BOOL WINAPI ConsoleHandler(DWORD dwType)
{
    switch(dwType) {
    case CTRL_C_EVENT:
    	printf("received SIGINT: will halt within %d ms ...\n", REFRESH_RATE);
        MUST_DIE = true;
        break;
    }

    return TRUE;
}

void SetBrightness(HDC hDC, WORD brightness)
{
	static WORD gamma_values[3][256];
	static WORD prev_brightness;

	if (brightness != prev_brightness) {
		for (int i = 0; i < 256; i++) {
			int gamma = i * (brightness + 128);

			if (gamma > 65535)
				gamma = 65535;

			gamma_values[0][i] =
			gamma_values[1][i] =
			gamma_values[2][i] = (WORD)gamma;
		}
		prev_brightness = brightness;
	}

	SetDeviceGammaRamp(hDC, gamma_values);
}

int EvaluateBrightness(float f)
{
	f = 1 - f;
	return (int) (256 * f * f * f);
}

int main(int argc, char const *argv[])
{
    if (!SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleHandler,TRUE)) {
        fprintf(stderr, "Unable to install handler!\n");
        return EXIT_FAILURE;
    }

	int height = GetSystemMetrics(SM_CYVIRTUALSCREEN);
	int width  = GetSystemMetrics(SM_CXVIRTUALSCREEN);

	int low_h = height / 4;
	int low_w = width  / 4;

	HDC hdc = GetDC(NULL);
	HDC hDest = CreateCompatibleDC(hdc);

	HBITMAP hbDesktop = CreateCompatibleBitmap( hdc, low_w, low_h);

	BITMAPINFO MyBMInfo = {0};
    MyBMInfo.bmiHeader.biSize = sizeof(MyBMInfo.bmiHeader);

	SelectObject(hDest, hbDesktop);
    GetDIBits(hdc, hbDesktop, 0, 0, NULL, &MyBMInfo, DIB_RGB_COLORS);

	BYTE* pixels  = new BYTE[MyBMInfo.bmiHeader.biSizeImage];

    MyBMInfo.bmiHeader.biCompression = BI_RGB;

	while (!MUST_DIE) {
		auto start = chrono::steady_clock::now();

		StretchBlt(hDest, 0, 0, low_w, low_h,
				   hdc, 0, 0, width, height, SRCCOPY);

	    GetDIBits(hdc, hbDesktop, 0, MyBMInfo.bmiHeader.biHeight,
	    	(LPVOID)pixels, &MyBMInfo, DIB_RGB_COLORS);

		float sum = 0;
		for (int x = 0; x < low_w; x++) {
			for (int y = 0; y < low_h; y++) {
				//0x00bbggrr
				COLORREF _bgr = pixels[x * low_w + y];
				float r = GetRValue(_bgr) / 255.0f;
				float g = GetGValue(_bgr) / 255.0f;
				float b = GetBValue(_bgr) / 255.0f;
				sum += (r+g+b) / 3.0f;
			}
		}
		sum = sum / (low_w*low_h);

		SetBrightness(hdc, EvaluateBrightness(sum));

	 	auto end = chrono::steady_clock::now();
		auto tt = chrono::duration <double, milli>(end - start).count();

		printf("b = %.1f %%, g = %d, t = %f ms, %d x %d\n",
			sum*100, EvaluateBrightness(sum), tt, low_w, low_h);

		Sleep(REFRESH_RATE - tt);
	}

	printf("shutting down\n");

	ReleaseDC(NULL, hdc);
	DeleteDC(hDest);
	delete[] pixels;

	SetBrightness(hdc, 128);

	return 0;
}