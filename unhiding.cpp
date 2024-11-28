#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <vector>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image_write.h"
#include "stb_image.h"

int main(){

    int x, y, n;

    // read image
    unsigned char *origin_image = stbi_load("./Lenna.png", &x, &y, &n, 0);
 
    // read lossy image
    unsigned char *lossy_image = stbi_load("./Lenna_lossy.jpg", &x, &y, &n, 0);

    // create deafult TER base
    int byte_size = x*y*n; 
    char *TER_base = new char[byte_size];
    for(int i = 0; i < byte_size; ++i){
        TER_base[i] = origin_image[i] - lossy_image[i];
    }

    // count received TER base
    unsigned char *received_image = stbi_load("./hiding.png", &x, &y, &n, 0);
    char *received_base = new char[byte_size];
    for(int i = 0; i < byte_size; ++i){
        received_base[i] = received_image[i] - origin_image[i];
    }

    // extract key chain
    std::vector<int> key_chain;
    for(int i = 0; i < byte_size; ++i){
        if(TER_base[i] != 0x00 && received_base[i] == 0x00){
            int zero_time = 1;
            for(int j = i + 1; j < byte_size; ++j){
                //std::cout<<"key"<<j<<std::endl;
                if(TER_base[j] == 0x00 || TER_base[j] == 0x01 || TER_base[j] == (char)-1) continue;
                if(received_base[j] == 0x00){
                    zero_time++;
                    continue;
                }
                if(zero_time > 4) break;
                key_chain.push_back((int)received_base[j]);
            }
            break;
        }
    }

    char* non01_deafult_base = new char[byte_size];
    char* non01_rece_base = new char[byte_size];
    int current = 0;
    for(int i = 0; i < byte_size; ++i){
        if(abs(TER_base[i]) >= 2){
            non01_deafult_base[current] = TER_base[i];
            non01_rece_base[current] = received_base[i];
            current++;
        }
    }


    // extract plain text
    unsigned char *plain_text = new unsigned char[key_chain.size()];
    int start = 0;
    for(int i = 0; i < key_chain.size(); ++i){
        unsigned char tmp = 0x00;
        for(int j = 0; j < key_chain[i]; ++j){
            unsigned char weight = 0x01;
            for(int k = j + 1; k <key_chain[i]; ++k){
                weight *= abs(non01_deafult_base[start + k]);
            }
            tmp += weight * (non01_deafult_base[start + j] - non01_rece_base[start + j]) * (non01_deafult_base[start + j] > 0 ? 1 : -1);
        }
        plain_text[i] = tmp;
        start += key_chain[i];
    }
    // for(int i = 0; i < key_chain.size(); ++i){
    //     int tmp = plain_text[i];
    //     std::cout<<tmp<<" ";
    // }
    
    std::cout<<plain_text<<std::endl;
}