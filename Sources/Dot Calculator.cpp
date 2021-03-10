#include <iostream>
#include <string>
#include <baseapi.h>
#include <windows.h>

using namespace std;

class Image
{
private:
    std::vector<std::uint8_t> Pixels;
    std::uint32_t width, height;
    std::uint16_t BitsPerPixel;
    void Flip(void* In, void* Out, int width, int height, unsigned int Bpp);

public:
    explicit Image(HDC DC, int X, int Y, int Width, int Height);

    inline std::uint16_t GetBitsPerPixel() { return this->BitsPerPixel; }
    inline std::uint16_t GetBytesPerPixel() { return this->BitsPerPixel / 8; }
    inline std::uint16_t GetBytesPerScanLine() { return (this->BitsPerPixel / 8) * this->width; }
    inline int GetWidth() const { return this->width; }
    inline int GetHeight() const { return this->height; }
    inline const std::uint8_t* GetPixels() { return this->Pixels.data(); }
};

//Banner, the text in Ascii Art
void Cookie_Monster_Banner() {
    std::cout << R"(   
__________________________________________________________________________________
_____/\\\\\\\\\_______/\\\\\\\\\______/\\\\\\\\\\\\\\\_____/\\\\\\\\\\\___________     
____/\\\\\\\\\\\\\___/\\\///////\\\___\/\\\///////////____/\\\/////////\\\________
____/\\\/////////\\\_\/\\\_____\/\\\___\/\\\______________\//\\\______\///________
____\/\\\_______\/\\\_\/\\\\\\\\\\\/____\/\\\\\\\\\\\_______\////\\\______________
_____\/\\\\\\\\\\\\\\\_\/\\\//////\\\____\/\\\///////___________\////\\\__________
______\/\\\/////////\\\_\/\\\____\//\\\___\/\\\_____________________\////\\\______
_______\/\\\_______\/\\\_\/\\\_____\//\\\__\/\\\______________/\\\______\//\\\____
________\/\\\_______\/\\\_\/\\\______\//\\\_\/\\\\\\\\\\\\\\\_\///\\\\\\\\\\\/____
_________\///________\///__\///________\///__\///////////////____\///////////_____  
__________________________________________________________________________________                                                                                                                                 
	)" << '\n';
}

void Image::Flip(void* In, void* Out, int width, int height, unsigned int Bpp)
{
    unsigned long Chunk = (Bpp > 24 ? width * 4 : width * 3 + width % 4);
    unsigned char* Destination = static_cast<unsigned char*>(Out);
    unsigned char* Source = static_cast<unsigned char*>(In) + Chunk * (height - 1);

    while (Source != In)
    {
        std::memcpy(Destination, Source, Chunk);
        Destination += Chunk;
        Source -= Chunk;
    }
}

Image::Image(HDC DC, int X, int Y, int Width, int Height) : Pixels(), width(Width), height(Height), BitsPerPixel(32)
{
    BITMAP Bmp = { 0 };
    HBITMAP hBmp = reinterpret_cast<HBITMAP>(GetCurrentObject(DC, OBJ_BITMAP));

    if (GetObject(hBmp, sizeof(BITMAP), &Bmp) == 0)
        throw std::runtime_error("BITMAP DC NOT FOUND.");

    RECT area = { X, Y, X + Width, Y + Height };
    HWND Window = WindowFromDC(DC);
    GetClientRect(Window, &area);

    HDC MemDC = GetDC(nullptr);
    HDC SDC = CreateCompatibleDC(MemDC);
    HBITMAP hSBmp = CreateCompatibleBitmap(MemDC, width, height);
    DeleteObject(SelectObject(SDC, hSBmp));

    BitBlt(SDC, 0, 0, width, height, DC, X, Y, SRCCOPY);
    unsigned int data_size = ((width * BitsPerPixel + 31) / 32) * 4 * height;
    std::vector<std::uint8_t> Data(data_size);
    this->Pixels.resize(data_size);

    BITMAPINFO Info = { sizeof(BITMAPINFOHEADER), static_cast<long>(width), static_cast<long>(height), 1, BitsPerPixel, BI_RGB, data_size, 0, 0, 0, 0 };
    GetDIBits(SDC, hSBmp, 0, height, &Data[0], &Info, DIB_RGB_COLORS);
    this->Flip(&Data[0], &Pixels[0], width, height, BitsPerPixel);

    DeleteDC(SDC);
    DeleteObject(hSBmp);
    ReleaseDC(nullptr, MemDC);
}

string startTessy() {
    
    HWND SomeWindowHandle = GetDesktopWindow();
    HDC DC = GetDC(SomeWindowHandle);

    Image Img = Image(DC, 0, 0, 2560, 1440);

    ReleaseDC(SomeWindowHandle, DC);

    char* outText;

    tesseract::TessBaseAPI* api = new tesseract::TessBaseAPI();

    api->Init("languagedata", "eng");
    api->SetImage(Img.GetPixels(), Img.GetWidth(), Img.GetHeight(), Img.GetBytesPerPixel(), Img.GetBytesPerScanLine());
    api->SetSourceResolution(300);
    api->SetRectangle(1190,100, 140, 54);
    api->SetPageSegMode(tesseract::PSM_SINGLE_LINE);
    api->SetVariable("tessedit_char_blacklist", "!?@#$%&*()<>_-+=/:;'\"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
    api->SetVariable("tessedit_char_whitelist", "0123456789");
    api->SetVariable("classify_bln_numeric_mode", "1");

    // Get OCR result
    outText = api->GetUTF8Text();

    string test = outText;

    // Destroy used object and release memory
    api->End();
    delete[] outText;

    return test;
}

void dotCalculation(int output) {

    int zero; //between 300 and 2400
    float dots;
    int range;

    for (int i = 2400; i >= 300; i = i-100)
    {
        int tempRange = output - i;
        if (tempRange > 0 || i == 300)
        {
            zero = i;
            range = tempRange;
            i = 0;
        }
        else
        {
            tempRange = 0;
        }
    }

    dots = (((0.0045 * pow((zero/100), 2.0) + 1)/100)*range)+0.1;

    system("cls");
    cout << "Range: " << output << "\n" << "Zero: " << zero << "\nDots: " << dots << endl;

}

int main()
{
    Cookie_Monster_Banner();

    string temp;
    string tempTemp = "";

    while (true)
    {
        string output = startTessy();
       
       if (output.length() == 5)
       {

           for (int i = 0; i < 4; i++)
           {
               if (output[i] == '1' || output[i] == '2' || output[i] == '3' || output[i] == '4' || output[i] == '5' || output[i] == '6' || output[i] == '7' || output[i] == '8' || output[i] == '9' || output[i] == '0')
               {
                   temp = temp + output[i];
               }
           }

           if (temp.length() == 4 && temp != tempTemp)
           {    
               dotCalculation(stoi(temp));
               tempTemp = temp;
           }

       }

       temp = "";

    }

    return 0;
}