/**
 * Build with:
gcc -o image_downloader image_downloader.c -lcurl
 *
*/

#include <stdio.h>
#include <curl/curl.h>
#include <string.h>

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}

int main(void) {
    CURL *curl;
    FILE *fp;
    CURLcode res;
    int i = 1;
    while (i <= 151) {
        for (int j = 1; j <= 2; j++) {
            char num[10];
            if (i < 10) {
                sprintf(num, "00%d", i);
            } else if (i < 100) {
                sprintf(num, "0%d", i);
            } else {
                sprintf(num, "%d", i);
            }
            char url[256];
            sprintf(url, "https://www.pokencyclopedia.info/sprites/gen3/spr_emerald/spr_e_%s_%d.png", num, j);
            char outfilename[50];
            sprintf(outfilename, "resources/full_pic/pokemon_%s_%d.png", num, j);
            curl = curl_easy_init();
            if (curl) {
                fp = fopen(outfilename,"wb");
                if (fp != NULL) {
                    curl_easy_setopt(curl, CURLOPT_URL, url);
                    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
                    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
                    res = curl_easy_perform(curl);
                    fclose(fp);
                }
                /* always cleanup */
                curl_easy_cleanup(curl);
            }
        }
        i++;
    }
    fclose(fp);
    return 0;
}