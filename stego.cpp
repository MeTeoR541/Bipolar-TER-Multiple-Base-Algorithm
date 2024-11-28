#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <vector>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image_write.h"
#include "stb_image.h"

int TER_multiple_base_algorithm(char** TER_base, int length, unsigned char* h, int text_size){
    std::vector<int> key_chain;
    int start = 0;
    
    for(int i = 0; i < text_size; ++i){
        int tmp = h[i];
        int j = start;
        int base = abs((int)(*TER_base[j]));
        int key = 1;
        while(tmp >= base){
            j++;
            base = base * abs((int)(*TER_base[j]));
            key++;
        }
        int converted_value = 0;
        for(int k = 0; k < key; ++k){
            converted_value = tmp % abs(*TER_base[j - k]);
            tmp = tmp / abs(*TER_base[j - k]);
            *TER_base[j - k] = (*TER_base[j - k]) - converted_value * ((*TER_base[j - k]) > 0 ? 1 : -1);
        }
        start = j + 1;
        key_chain.push_back(key);
    }
    if(length - start >= 4 + key_chain.size()){
        for(int i = 0; i < 4 + key_chain.size(); ++i){
            if(i < 4){
                *TER_base[start] = 0;
            }
            else{
                *TER_base[start] = key_chain[i - 4];
                int tmp = *TER_base[start];
                std::cout<<tmp<<" ";
            }
            start++;
        }
        if(start < length)
            *TER_base[start] = 0;
        std::cout<<"End data "<<start<<std::endl;
    }
    else{
        std::cerr<<"image space isn't enough.\n";
        return -1;
    }
    return 1;
}

int main(){
    
    int size = 0;

    // read plain text
    std::ifstream file("./plain_text.txt", std::ios::binary);
    size = std::filesystem::file_size("./plain_text.txt");
    unsigned char *plain_text = new unsigned char[size];
    file.read(reinterpret_cast<char*>(plain_text), size);
    file.close();
    for(int i = 0; i < size; ++i){
        int tmp = plain_text[i];
        std::cout<<tmp<<" ";
    }
    std::cout<<plain_text<<"\n";

    int x, y, n;
    
    // read image
    int origin_size = 0;
    origin_size = std::filesystem::file_size("./Lenna.png");
    unsigned char *origin_image = stbi_load("./Lenna.png", &x, &y, &n, 0);
 

    // read lossy image
    int lossy_size = 0;
    lossy_size = std::filesystem::file_size("./Lenna_lossy.jpg");
    unsigned char *lossy_image = stbi_load("./Lenna_lossy.jpg", &x, &y, &n, 0);


    // create deafult TER base
    int byte_size = x*y*n; 
    char *TER_base = new char[byte_size];
    for(int i = 0; i < byte_size; ++i){
        TER_base[i] = origin_image[i] - lossy_image[i];
    } 

    // for(int i = 0; i < 50; ++i){
    //     int tmp = TER_base[i];
    //     std::cout<<tmp<<" ";
    // }
    // std::cout<<std::endl;

    char **non_zero_one_TER_base = new char*[byte_size];
    int j = 0; 
    for(int i = 0; i < byte_size; ++i){
        if(abs(TER_base[i]) >= 2){
            non_zero_one_TER_base[j] = &TER_base[i];
            j++;
        }
    }

    if(TER_multiple_base_algorithm(non_zero_one_TER_base, j, plain_text, size) < 0){
        std::cerr<<"Error.\n";
    }

    // for(int i = 0; i < 50; ++i){
    //     int tmp = TER_base[i];
    //     std::cout<<tmp<<" ";
    // }
    // std::cout<<std::endl;
    
    for(int i = 0; i < byte_size; ++i){
        origin_image[i] = origin_image[i] + TER_base[i];
    }
    stbi_write_png("hiding.png", x, y, n, (void*)origin_image, 0);
  
}