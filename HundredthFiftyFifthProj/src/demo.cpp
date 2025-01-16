/**
 * @file demo.cpp
 * @author wysaid (this@xege.org)
 * @brief չʾ���ʹ�� EGE ʵ��һ���򵥵Ľ�ͼ����.
 */

#include <ege_graphics.h>

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

int main()
{
    SetProcessDPIAware(); /// ����ϵͳ DPI �趨, �����ͼ.

    setinitmode(INIT_NOBORDER | INIT_TOPMOST);
    /// ��ȡ��Ļ HDC
    HDC screenHDC = GetDC(NULL);

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);  /// �߼���Ļ���
    int screenHeight = GetSystemMetrics(SM_CYSCREEN); /// �߼���Ļ�߶�

    /// һ��ʼֻʹ��һ����С�ֱ���, ��ΪҪ��������. ������Ļ��˸.
    initgraph(1, 1);
    HWND windowHwnd = getHWnd();
    /// ���ش���
    ShowWindow(windowHwnd, SW_HIDE);

    setrendermode(RENDER_MANUAL);

    PIMAGE img = newimage(screenWidth, screenHeight);
    HDC imgHDC = getHDC(img);

    /// ����Ļ��ͼ
    BitBlt(imgHDC, 0, 0, screenWidth, screenHeight, screenHDC, 0, 0, SRCCOPY);

    /// �ͷ���Դ, ���ǽ�ͼ����ִֻ��һ�ξ��˳�, �����������������ʵ���ͷ�Ҳû��.
    // ReleaseDC(NULL, screenHDC);

    resizewindow(screenWidth, screenHeight);

    putimage_alphablend(NULL, img, 0, 0, 0x50, 0, 0, screenWidth, screenHeight);

    /// ǿ��ˢ��һ����Ļ.
    Sleep(0);
    /// �Ȼ���һ��, ����ʾ����, ��������
    ShowWindow(windowHwnd, SW_SHOW);

    bool leftDown = false;
    bool hasSelection = false;
    int startX = 0, startY = 0;
    int endX = 0, endY = 0;
    int lastMouseX = 0, lastMouseY = 0;

    /// ����һ�������õĳ���, ��Ӧ��Ƶ��ˢ��.
    /// ��������һ�����±��, ��ֹ����ˢ�¿պ�cpu.
    bool shouldUpdate = true;

    /// ����һ����ʾ����
    setbkmode(TRANSPARENT);
    setfont(20, 10, "����");

    /// ��ѭ��
    for (; is_run(); delay_fps(60))
    {
        while (mousemsg())
        {
            mouse_msg msg = getmouse();

            shouldUpdate = true;
            switch (msg.msg)
            {
            case mouse_msg_down:
                if (msg.is_left())
                {
                    leftDown = true;
                    lastMouseX = msg.x;
                    lastMouseY = msg.y;

                    // ����µ������λ����֮ǰ��ѡ����, ����Ϊ���ƶ�ѡ��.
                    if (hasSelection && msg.x >= startX && msg.x <= endX && msg.y >= startY && msg.y <= endY)
                    {
                        break;
                    }

                    startX = msg.x;
                    startY = msg.y;
                    endX = msg.x;
                    endY = msg.y;
                    hasSelection = false;
                }
                else if (msg.is_right())
                {
                    /// ��������Ҽ�, �����ѡ��, ��ȥ��ѡ��, ����ֱ���˳�����.
                    if (hasSelection)
                    {
                        hasSelection = false;
                        startX = endX = startY = endY = 0;
                        continue;
                    }
                    return 0;
                }
                else if (msg.is_mid())
                { /// ��������м�, ִ�н�ͼ�������
                    if (hasSelection)
                    { /// ���������ͼ.
                        int w = abs(endX - startX);
                        int h = abs(endY - startY);
                        int left = MIN(startX, endX);
                        int top = MIN(startY, endY);
                        PIMAGE selection = newimage(w, h);
                        getimage(selection, img, left, top, w, h);

                        char filename[MAX_PATH] = { 0 };
                        OPENFILENAME ofn = { 0 };
                        ofn.lStructSize = sizeof(ofn);
                        ofn.hwndOwner = windowHwnd;
                        ofn.lpstrFile = filename;
                        ofn.nMaxFile = sizeof(filename);
                        ofn.lpstrFilter = "PNG\0*.png\0";
                        ofn.lpstrTitle = "�����ͼ";
                        ofn.Flags = OFN_EXPLORER | OFN_OVERWRITEPROMPT;
                        if (GetSaveFileName(&ofn))
                        {
                            // ���filename��׺���� .png, ���Զ�����.
                            char* ext = strrchr(filename, '.');
                            if (!ext || strcmp(ext, ".png") != 0)
                            {
                                strncat(filename, ".png", sizeof(filename) - strlen(filename) - 1);
                            }
                            savepng(selection, filename);
                        }
                        return 0;
                    }
                }
                break;
            case mouse_msg_up:
                if (msg.is_left())
                {
                    leftDown = false;
                    if (endX != startX && endY != startY)
                        hasSelection = true;
                }

                break;
            case mouse_msg_move:
                if (leftDown)
                {
                    if (hasSelection)
                    {
                        int offsetX = msg.x - lastMouseX;
                        int offsetY = msg.y - lastMouseY;
                        startX += offsetX;
                        startY += offsetY;
                        endX += offsetX;
                        endY += offsetY;
                    }
                    else
                    {
                        endX = msg.x;
                        endY = msg.y;
                    }
                    lastMouseX = msg.x;
                    lastMouseY = msg.y;
                }
                break;
            }
        }

        while (kbhit())
        { /// ���̰��� esc ��, �˳�����.
            if (getch() == key_esc)
            {
                return 0;
            }
        }

        if (shouldUpdate)
        {
            shouldUpdate = false;
            cleardevice();
            putimage_alphablend(NULL, img, 0, 0, 0x50, 0, 0, screenWidth, screenHeight);

            /// ����һ����ɫ�ľ��ο�, ��ʾ������Ļ.
            setcolor(RED);
            setlinewidth(4);
            rectangle(2, 2, screenWidth - 4, screenHeight - 4);

            if (endX == startX || endY == startY)
                continue; /// �������ѡ��.

            int w = abs(endX - startX);
            int h = abs(endY - startY);
            int left = MIN(startX, endX);
            int top = MIN(startY, endY);
            putimage(left, top, w, h, img, left, top, w, h);
            /// ����һ����ɫ�ľ��ο�, ��ʾѡ��.
            setcolor(WHITE);
            setlinewidth(2);
            rectangle(left, top, left + w, top + h);

            /// ����һ����ʾ��
            setcolor(YELLOW);
            outtextxy(left, top + h, "��ESC���˳�, �Ҽ�ȡ��, ��������м�(����)��������.");
        }
    }

    return 0;
}