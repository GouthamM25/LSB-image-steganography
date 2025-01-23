#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "decode.h"

int main(int argc,char *argv[])
{
    EncodeInfo encInfo;
    DecodeInfo decInfo;

    if(argc==1)
    {
        printf("./a.out: Encoding: ./a.out -e <.bmp file> <.txt file> [output file]\n");//printing error message
        printf("./a.out: Decoding: ./a.out -d <.bmp file> [output file]\n");//printing error message
        return e_failure;
    }

    //to find operation type
    if(check_operation_type(argv[1]) == e_encode)
    {
        //validating command line argument length
        if(argc<4)
        {
            printf("./a.out: Encoding: ./a.out -e <.bmp file> <.txt file> [output file]\n");//error message
            return e_failure;
        }
         if(argc == 5)
         {
            if(strstr(argv[4],".bmp") == NULL)//checking if entered argument contains .bmp or not
            {
                printf("INFO: %s is not a .bmp file!\n",argv[4]);//error message
                return e_failure;
            }
            char *str=strchr(argv[4],'.');
            printf("%s",str);
            if(strcmp(str,".bmp")!=0)
            {
                printf("INFO: %s is not a .bmp file!\n",argv[4]);
                return e_failure;
            }
            encInfo.stego_image_fname = argv[4];
        }
        else
        {
            encInfo.stego_image_fname = "steged_img.bmp";//creating default file_name for encoding image
        }

        if((strstr(argv[2],".bmp")) == NULL)
        {
            printf("INFO :%s is not a .bmp file!\n",argv[2]);//error message
            return e_failure;
        }
        encInfo.src_image_fname = argv[2];

        if(strchr(argv[3],'.') == NULL)
        {
            printf("INFO: %s no extension mentioned!\n",argv[3]);//error message
            return e_failure;
        }
        encInfo.secret_fname = argv[3];
         if(argc == 4)
        {
            printf("INFO: Output File not mentioned. Creating steged_img.bmp as default\n");
        }
        
        //start encoding after validation 
        if(do_encoding(&encInfo) == e_success)
        {
            printf("INFO: ## Encoding Done Successfully ##\n");//printing encoding success
        }
    }

    //validating command line argument 
    else if(check_operation_type(argv[1]) == e_decode)
    {
        if(argc<3)
        {
            printf("./a.out: Decoding: ./a.out -d <.bmp file> [output file]\n");//errorr message
        }
         if(argc == 4){
            decInfo.file_name = strtok(argv[3],".");
        }
        else
        {
            decInfo.file_name = "decoded";//default file name for output
            decInfo.file_present = 1;//flag
        }
        
        if(strstr(argv[2],".bmp") == NULL)
        {
            printf("INFO: %s is not a .bmp file!\n",argv[2]);//error message
            return e_failure;
        }
        decInfo.stego_image_fname = argv[2];
        printf("INFO: ## Decoding Procedure Started ##\n");
        //started decoding after validation
        if(do_decoding(&decInfo) == e_success)
        {
            printf("## Decoding Done Successfully ##\n");
        }

    }

    else
    {
        /*invalid*/
        printf("Invalid operation!\nGive -e for encoding or -d for decoding\n");//unsupported error message
    } 
    return 0;
}

OperationType check_operation_type(char *argv)
{
    if(strcmp(argv,"-e") == 0)
    {
        return e_encode;
    }
    else if(strcmp(argv,"-d") == 0)
    {
        return e_decode;
    }
    else
    {
        return e_unsupported;
    }
}
