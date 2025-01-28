#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <curl/curl.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#define GRID_SIZE 20
#define CELL_SIZE 30

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
TTF_Font *font = NULL;


SDL_Color humanColor = {0, 0, 255, 255};
SDL_Color orcColor = {255, 0, 0, 255};
SDL_Color emptyColor = {255, 255, 255, 255};


SDL_Color healthFull = {0, 255, 0, 255};
SDL_Color healthMedium = {255, 255, 0, 255};
SDL_Color healthLow = {255, 0, 0, 255};


SDL_Texture *textureHumanPikemen = NULL;
SDL_Texture *textureHumanArchers = NULL;
SDL_Texture *textureHumanCavalry = NULL;
SDL_Texture *textureHumanSiege = NULL;

SDL_Texture *textureOrcWarriors = NULL;
SDL_Texture *textureOrcSpearmen = NULL;
SDL_Texture *textureOrcWargRiders = NULL;
SDL_Texture *textureOrcTrolls = NULL;


struct Birim
{

    int saldiri_gucu ;
    int savunma_gucu ;
    int saglik ;
    double kritik_sans ;
    int sayi ;
    double toplam_saldiri_gucu ;
    double toplam_savunma_gucu ;
    double guncel_saldiri_gucu ;
    double guncel_savunma_gucu ;
    double guncel_saglik ;
    int saldiri_sayaci ;
    int kritik_vurus_oldu_mu ;
    int ilk_sayi ;
};

struct seviye{

    int aktiflik_durumu ;
    int deger ;
    char aciklama[100];

};
struct arastirma_turu{

    int arturu_aktiflik ;
    struct seviye seviye1 ;
    struct seviye seviye2 ;
    struct seviye seviye3 ;
};
struct arastirma{

    struct arastirma_turu savunma_ustaligi ;
    struct arastirma_turu saldiri_gelistirmesi ;
    struct arastirma_turu elit_egitim ;
    struct arastirma_turu kusatma_ustaligi ;

};

struct hero
{

    char bonus_turu[25];
    char aciklama[100] ;
    int bonus_degeri ;
    int aktiflik_durumu  ;
};

struct creature
{

    int etki_degeri ;
    int aktiflik_durumu ;
    char etki_turu[15];
    char aciklama[100] ;

};


struct taraf1
{

    struct Birim piyadeler ;
    struct Birim okcular  ;
    struct Birim suvariler;
    struct Birim kusatma_makineleri ;
    struct hero Alparslan ;
    struct hero Fatih_Sultan_Mehmet ;
    struct hero Yavuz_Sultan_Selim  ;
    struct hero Tugrul_Bey ;
    struct hero Mete_Han ;
    struct creature Ejderha ;
    struct creature Agri_Dagi_Devleri ;
    struct creature Tepegoz ;
    struct creature Karakurt ;
    struct creature Samur ;
    struct arastirma insan_arastirmalari ;
};

struct taraf2
{

    struct Birim ork_dovusculeri ;
    struct Birim mizrakcilar ;
    struct Birim varg_binicileri ;
    struct Birim troller ;
    struct hero Goruk_Vahsi ;
    struct hero Thruk_Kemikkiran ;
    struct hero Vrog_Kafakiran ;
    struct hero Ugar_Zalim ;
    struct creature Kara_Troll ;
    struct creature Golge_Kurtlari ;
    struct creature Camur_Devleri ;
    struct creature Ates_Iblisi ;
    struct creature Buz_Devleri ;
    struct creature Makrog_Savas_Beyi ;
    struct arastirma ork_arastirmalari ;

};


struct Hafiza {

    char *memory ;
    size_t boyut ;
};

typedef enum {
    EMPTY,
    HUMAN_PIKEMEN,
    HUMAN_ARCHERS,
    HUMAN_CAVALRY,
    HUMAN_SIEGE,
    ORC_WARRIORS,
    ORC_SPEARMEN,
    ORC_WARG_RIDERS,
    ORC_TROLLS
} UnitType ;


typedef struct {
    UnitType type;
    int count;
    int originalCount;
    double healthPercentage;
    SDL_Color color;
    SDL_Texture *texture;
} GridCell;

GridCell grid[GRID_SIZE][GRID_SIZE];


void clearGrid() {
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            grid[i][j].type = EMPTY;
            grid[i][j].count = 0;
            grid[i][j].originalCount = 0;
            grid[i][j].healthPercentage = 1.0;
            grid[i][j].color = emptyColor;
        }
    }
}

SDL_Texture *loadTexture(const char *path) {
    SDL_Texture *newTexture = NULL;
    SDL_Surface *loadedSurface = IMG_Load(path);
    if (loadedSurface == NULL) {
        printf("Resim yüklenemedi! SDL_image Hatasý: %s\n", IMG_GetError());
    } else {
        newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
        if (newTexture == NULL) {
            printf("Texture oluþturulamadý! SDL Hatasý: %s\n", SDL_GetError());
        }
        SDL_FreeSurface(loadedSurface);
    }
    return newTexture;
}



void placeUnitsOnGrid(struct taraf1 *insan, struct taraf2 *ork) {
    clearGrid();



    int humanRow = 0;
    int humanCol = 0;


    struct {
        UnitType type;
        int count;
        double healthPercentage;
        SDL_Color color;
    } humanUnits[] = {
        {HUMAN_PIKEMEN, insan->piyadeler.sayi, 1.0, humanColor},
        {HUMAN_ARCHERS, insan->okcular.sayi, 1.0, humanColor},
        {HUMAN_CAVALRY, insan->suvariler.sayi, 1.0, humanColor},
        {HUMAN_SIEGE, insan->kusatma_makineleri.sayi, 1.0, humanColor}
    };


    for (int i = 0; i < 4; i++) {
        int remaining = humanUnits[i].count;
        while (remaining > 0 && humanRow < GRID_SIZE) {
            if (humanCol >= GRID_SIZE) {
                humanCol = 0;
                humanRow++;
            }

    switch (humanUnits[i].type) {
        case HUMAN_PIKEMEN:
            grid[humanRow][humanCol].texture = textureHumanPikemen;
                break;
        case HUMAN_ARCHERS:
            grid[humanRow][humanCol].texture = textureHumanArchers;
                break;
        case HUMAN_CAVALRY:
            grid[humanRow][humanCol].texture = textureHumanCavalry;
                break;
        case HUMAN_SIEGE:
            grid[humanRow][humanCol].texture = textureHumanSiege;
                break;
        default:
            grid[humanRow][humanCol].texture = NULL;
                break;
}

            int unitsToPlace = remaining > 100 ? 100 : remaining;
            grid[humanRow][humanCol].type = humanUnits[i].type;
            grid[humanRow][humanCol].count = unitsToPlace;
            grid[humanRow][humanCol].originalCount = unitsToPlace;
            grid[humanRow][humanCol].healthPercentage = humanUnits[i].healthPercentage;
            grid[humanRow][humanCol].color = humanUnits[i].color;
            remaining -= unitsToPlace;
            humanCol++;
        }
    }





    int orcRow = GRID_SIZE - 1;
    int orcCol = GRID_SIZE - 1;


    struct {
        UnitType type;
        int count;
        double healthPercentage;
        SDL_Color color;
    } orcUnits[] = {
        {ORC_WARRIORS, ork->ork_dovusculeri.sayi, 1.0, orcColor},
        {ORC_SPEARMEN, ork->mizrakcilar.sayi, 1.0, orcColor},
        {ORC_WARG_RIDERS, ork->varg_binicileri.sayi, 1.0, orcColor},
        {ORC_TROLLS, ork->troller.sayi, 1.0, orcColor}
    };


    for (int i = 0; i < 4; i++) {
        int remaining = orcUnits[i].count;
        while (remaining > 0 && orcRow >= 0) {
            if (orcCol < 0) {
                orcCol = GRID_SIZE - 1;
                orcRow--;
            }

            switch (orcUnits[i].type) {
        case ORC_WARRIORS:
            grid[orcRow][orcCol].texture = textureOrcWarriors;
                break;
        case ORC_SPEARMEN:
            grid[orcRow][orcCol].texture = textureOrcSpearmen;
                break;
        case ORC_WARG_RIDERS:
            grid[orcRow][orcCol].texture = textureOrcWargRiders;
                break;
        case ORC_TROLLS:
            grid[orcRow][orcCol].texture = textureOrcTrolls;
                break;
        default:
            grid[orcRow][orcCol].texture = NULL;
                break;
}
            int unitsToPlace = remaining > 100 ? 100 : remaining;
            grid[orcRow][orcCol].type = orcUnits[i].type;
            grid[orcRow][orcCol].count = unitsToPlace;
            grid[orcRow][orcCol].originalCount = unitsToPlace;
            grid[orcRow][orcCol].healthPercentage = orcUnits[i].healthPercentage;
            grid[orcRow][orcCol].color = orcUnits[i].color;
            remaining -= unitsToPlace;
            orcCol--;
        }
    }


}


void drawText(const char *text, int x, int y, SDL_Color color) {
    SDL_Surface *textSurface = TTF_RenderText_Blended(font, text, color);
    if (textSurface != NULL) {
        SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_Rect renderQuad = {x, y, textSurface->w, textSurface->h};
        SDL_RenderCopy(renderer, textTexture, NULL, &renderQuad);
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
    }
}



void drawGrid() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);


    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    for (int i = 0; i <= GRID_SIZE; i++) {

        SDL_RenderDrawLine(renderer, i * CELL_SIZE, 0, i * CELL_SIZE, GRID_SIZE * CELL_SIZE);

        SDL_RenderDrawLine(renderer, 0, i * CELL_SIZE, GRID_SIZE * CELL_SIZE, i * CELL_SIZE);
    }


    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            GridCell cell = grid[i][j];
            if (cell.type != EMPTY && cell.texture != NULL) {

                SDL_Rect rect = {j * CELL_SIZE, i * CELL_SIZE, CELL_SIZE, CELL_SIZE};
                SDL_RenderCopy(renderer, cell.texture, NULL, &rect);


                SDL_Color healthColor;
                if (cell.healthPercentage > 0.5) {
                    healthColor = healthFull;
                } else if (cell.healthPercentage > 0.2) {
                    healthColor = healthMedium;
                } else {
                    healthColor = healthLow;
                }

                SDL_SetRenderDrawColor(renderer, healthColor.r, healthColor.g, healthColor.b, healthColor.a);
                SDL_Rect healthRect = {
                    j * CELL_SIZE,
                    i * CELL_SIZE + CELL_SIZE - 5,
                    (int)(CELL_SIZE * cell.healthPercentage),
                    5};
                SDL_RenderFillRect(renderer, &healthRect);


                char countText[10];
                snprintf(countText, 10, "%d", cell.count);


                drawText(countText, j * CELL_SIZE + CELL_SIZE / 4, i * CELL_SIZE + CELL_SIZE / 4, (SDL_Color){255, 255, 255, 255});
            }
        }
    }
    SDL_RenderPresent(renderer);
}



int initSDL()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL baslatilamadi! SDL_Error: %s\n", SDL_GetError());
        return 0;
    }


    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        printf("SDL_image baslatilamadi! SDL_image Hatasi: %s\n", IMG_GetError());
        return 0;
    }


    if (TTF_Init() == -1) {
        printf("TTF baslatilamadi! SDL_ttf Hatasi: %s\n", TTF_GetError());
        return 0;
    }


    window = SDL_CreateWindow("Savas Simülasyonu",
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              GRID_SIZE * CELL_SIZE,
                              GRID_SIZE * CELL_SIZE + 100,
                              SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("Pencere olusturulamadi! SDL_Error: %s\n", SDL_GetError());
        return 0;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("Renderer olusturulamadi! SDL_Error: %s\n", SDL_GetError());
        return 0;
    }


    font = TTF_OpenFont("arial.ttf", 16);
    if (font == NULL) {
        printf("Font yüklenemedi! SDL_ttf Hatasi: %s\n", TTF_GetError());
        return 0;
    }

    textureHumanPikemen = loadTexture("C:\\Users\\user\\Desktop\\prolab_images\\kilicc.jpg");
    textureHumanArchers = loadTexture("C:\\Users\\user\\Desktop\\prolab_images\\okcu.jpeg");
    textureHumanCavalry = loadTexture("C:\\Users\\user\\Desktop\\prolab_images\\suvari.jpeg");
    textureHumanSiege = loadTexture("C:\\Users\\user\\Desktop\\prolab_images\\kusatma.jpeg");

    textureOrcWarriors = loadTexture("C:\\Users\\user\\Desktop\\prolab_images\\barbar.jpeg");
    textureOrcSpearmen = loadTexture("C:\\Users\\user\\Desktop\\prolab_images\\mizrakci.jpg");
    textureOrcWargRiders = loadTexture("C:\\Users\\user\\Desktop\\prolab_images\\varg.jpeg");
    textureOrcTrolls = loadTexture("C:\\Users\\user\\Desktop\\prolab_images\\troller.jpeg");


    if (!textureHumanPikemen || !textureHumanArchers || !textureHumanCavalry || !textureHumanSiege ||
    !textureOrcWarriors || !textureOrcSpearmen || !textureOrcWargRiders || !textureOrcTrolls) {
    printf("Birim resimleri yüklenirken hata olustu.\n");
    return 0;


    return 1;
}


}


void closeSDL() {

    SDL_DestroyTexture(textureHumanPikemen);
    SDL_DestroyTexture(textureHumanArchers);
    SDL_DestroyTexture(textureHumanCavalry);
    SDL_DestroyTexture(textureHumanSiege);
    SDL_DestroyTexture(textureOrcWarriors);
    SDL_DestroyTexture(textureOrcSpearmen);
    SDL_DestroyTexture(textureOrcWargRiders);
    SDL_DestroyTexture(textureOrcTrolls);


    TTF_CloseFont(font);
    font = NULL;
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    IMG_Quit();
}


void updateHealthPercentages(struct taraf1 *insan, struct taraf2 *ork) {

    double piyadeHealthPercentage = insan->piyadeler.ilk_sayi > 0 ? ((double)insan->piyadeler.sayi) / insan->piyadeler.ilk_sayi : 0.0;
    double okcuHealthPercentage = insan->okcular.ilk_sayi > 0 ? ((double)insan->okcular.sayi) / insan->okcular.ilk_sayi : 0.0;
    double suvariHealthPercentage = insan->suvariler.ilk_sayi > 0 ? ((double)insan->suvariler.sayi) / insan->suvariler.ilk_sayi : 0.0;
    double kusatmaHealthPercentage = insan->kusatma_makineleri.ilk_sayi > 0 ? ((double)insan->kusatma_makineleri.sayi) / insan->kusatma_makineleri.ilk_sayi : 0.0;

    double orkDovuscuHealthPercentage = ork->ork_dovusculeri.ilk_sayi > 0 ? ((double)ork->ork_dovusculeri.sayi) / ork->ork_dovusculeri.ilk_sayi : 0.0;
    double mizrakciHealthPercentage = ork->mizrakcilar.ilk_sayi > 0 ? ((double)ork->mizrakcilar.sayi) / ork->mizrakcilar.ilk_sayi : 0.0;
    double vargBiniciHealthPercentage = ork->varg_binicileri.ilk_sayi > 0 ? ((double)ork->varg_binicileri.sayi) / ork->varg_binicileri.ilk_sayi : 0.0;
    double trolHealthPercentage = ork->troller.ilk_sayi > 0 ? ((double)ork->troller.sayi) / ork->troller.ilk_sayi : 0.0;


    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            GridCell *cell = &grid[i][j];
            if (cell->type != EMPTY) {
                switch (cell->type) {
                    case HUMAN_PIKEMEN:
                        cell->healthPercentage = piyadeHealthPercentage;
                        cell->count = (int)(cell->originalCount * piyadeHealthPercentage);
                        break;
                    case HUMAN_ARCHERS:
                        cell->healthPercentage = okcuHealthPercentage;
                        cell->count = (int)(cell->originalCount * okcuHealthPercentage);
                        break;
                    case HUMAN_CAVALRY:
                        cell->healthPercentage = suvariHealthPercentage;
                        cell->count = (int)(cell->originalCount * suvariHealthPercentage);
                        break;
                    case HUMAN_SIEGE:
                        cell->healthPercentage = kusatmaHealthPercentage;
                        cell->count = (int)(cell->originalCount * kusatmaHealthPercentage);
                        break;
                    case ORC_WARRIORS:
                        cell->healthPercentage = orkDovuscuHealthPercentage;
                        cell->count = (int)(cell->originalCount * orkDovuscuHealthPercentage);
                        break;
                    case ORC_SPEARMEN:
                        cell->healthPercentage = mizrakciHealthPercentage;
                        cell->count = (int)(cell->originalCount * mizrakciHealthPercentage);
                        break;
                    case ORC_WARG_RIDERS:
                        cell->healthPercentage = vargBiniciHealthPercentage;
                        cell->count = (int)(cell->originalCount * vargBiniciHealthPercentage);
                        break;
                    case ORC_TROLLS:
                        cell->healthPercentage = trolHealthPercentage;
                        cell->count = (int)(cell->originalCount * trolHealthPercentage);
                        break;
                    default:
                        cell->healthPercentage = 0.0;
                        cell->count = 0;
                        break;
                }
            }
        }
    }
}






static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct Hafiza *mem = (struct Hafiza *)userp;

    char *ptr = realloc(mem->memory, mem->boyut + realsize + 1);
    if (ptr == NULL) {
        printf("Yeterli alan yok(pointer null dondu.)\n");
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->boyut]), contents, realsize);
    mem->boyut += realsize;
    mem->memory[mem->boyut] = 0;

    return realsize;
}

void ilk_deger_atama(struct taraf1 *insan,struct taraf2 *ork){


    insan->piyadeler.saldiri_gucu = 0;
    insan->piyadeler.savunma_gucu = 0;
    insan->piyadeler.saglik = 0;
    insan->piyadeler.kritik_sans = 0.00;

    insan->okcular.saldiri_gucu = 0;
    insan->okcular.savunma_gucu = 0;
    insan->okcular.saglik = 0;
    insan->okcular.kritik_sans = 0.00;

    insan->suvariler.saldiri_gucu = 0;
    insan->suvariler.savunma_gucu = 0;
    insan->suvariler.saglik = 0;
    insan->suvariler.kritik_sans = 0.00;

    insan->kusatma_makineleri.saldiri_gucu = 0;
    insan->kusatma_makineleri.savunma_gucu = 0;
    insan->kusatma_makineleri.saglik = 0;
    insan->kusatma_makineleri.kritik_sans = 0.00 ;


    ork->ork_dovusculeri.saldiri_gucu = 0;
    ork->ork_dovusculeri.savunma_gucu = 0;
    ork->ork_dovusculeri.saglik = 0;
    ork->ork_dovusculeri.kritik_sans = 0.00;

    ork->mizrakcilar.saldiri_gucu = 0;
    ork->mizrakcilar.savunma_gucu = 0;
    ork->mizrakcilar.saglik = 0;
    ork->mizrakcilar.kritik_sans = 0.00;

    ork->varg_binicileri.saldiri_gucu = 0;
    ork->varg_binicileri.savunma_gucu = 0;
    ork->varg_binicileri.saglik = 0;
    ork->varg_binicileri.kritik_sans = 0.00;

    ork->troller.saldiri_gucu = 0;
    ork->troller.savunma_gucu = 0;
    ork->troller.saglik = 0;
    ork->troller.kritik_sans = 0.00;

    insan->Alparslan.bonus_degeri = 0;
    insan->Alparslan.aktiflik_durumu = 0;
    strcpy(insan->Alparslan.aciklama, " ");
    strcpy(insan->Alparslan.bonus_turu, " ");

    insan->Fatih_Sultan_Mehmet.aktiflik_durumu = 0 ;
    insan->Fatih_Sultan_Mehmet.bonus_degeri = 0  ;
    strcpy(insan->Fatih_Sultan_Mehmet.aciklama, " ");
    strcpy(insan->Fatih_Sultan_Mehmet.bonus_turu, " ");

    insan->Yavuz_Sultan_Selim.aktiflik_durumu = 0  ;
    insan->Yavuz_Sultan_Selim.bonus_degeri = 0  ;
    strcpy(insan->Yavuz_Sultan_Selim.aciklama, " ");
    strcpy(insan->Yavuz_Sultan_Selim.bonus_turu, " ");

    insan->Tugrul_Bey.aktiflik_durumu = 0  ;
    insan->Tugrul_Bey.bonus_degeri = 0  ;
    strcpy(insan->Tugrul_Bey.aciklama, " ");
    strcpy(insan->Tugrul_Bey.bonus_turu, " ");

    insan->Mete_Han.aktiflik_durumu = 0  ;
    insan->Mete_Han.bonus_degeri = 0  ;
    strcpy(insan->Mete_Han.aciklama, " ");
    strcpy(insan->Mete_Han.bonus_turu, " ");

    ork->Goruk_Vahsi.aktiflik_durumu = 0 ;
    ork->Goruk_Vahsi.bonus_degeri = 0 ;
    strcpy(ork->Goruk_Vahsi.aciklama, " ");
    strcpy(ork->Goruk_Vahsi.bonus_turu, " ");

    ork->Thruk_Kemikkiran.aktiflik_durumu = 0 ;
    ork->Thruk_Kemikkiran.bonus_degeri = 0 ;
    strcpy(ork->Thruk_Kemikkiran.aciklama, " ");
    strcpy(ork->Thruk_Kemikkiran.bonus_turu, " ");

    ork->Vrog_Kafakiran.aktiflik_durumu = 0 ;
    ork->Vrog_Kafakiran.bonus_degeri = 0 ;
    strcpy(ork->Vrog_Kafakiran.aciklama, " ");
    strcpy(ork->Vrog_Kafakiran.bonus_turu, " ");

    ork->Ugar_Zalim.aktiflik_durumu = 0 ;
    ork->Ugar_Zalim.bonus_degeri = 0 ;
    strcpy(ork->Ugar_Zalim.aciklama, " ");
    strcpy(ork->Ugar_Zalim.bonus_turu, " ");

    insan->Ejderha.aktiflik_durumu = 0 ;
    insan->Ejderha.etki_degeri = 0 ;
    strcpy(insan->Ejderha.etki_turu, " ");
    strcpy(insan->Ejderha.aciklama, " ");

    insan->Agri_Dagi_Devleri.aktiflik_durumu = 0 ;
    insan->Agri_Dagi_Devleri.etki_degeri = 0 ;
    strcpy(insan->Agri_Dagi_Devleri.etki_turu, " ");
    strcpy(insan->Agri_Dagi_Devleri.aciklama, " ");

    insan->Tepegoz.aktiflik_durumu = 0 ;
    insan->Tepegoz.etki_degeri = 0 ;
    strcpy(insan->Tepegoz.etki_turu, " ");
    strcpy(insan->Tepegoz.aciklama, " ");

    insan->Karakurt.aktiflik_durumu = 0 ;
    insan->Karakurt.etki_degeri = 0 ;
    strcpy(insan->Karakurt.etki_turu, " ");
    strcpy(insan->Karakurt.aciklama, " ");

    insan->Samur.aktiflik_durumu = 0 ;
    insan->Samur.etki_degeri = 0 ;
    strcpy(insan->Samur.etki_turu, " ");
    strcpy(insan->Samur.aciklama, " ");

    ork->Kara_Troll.aktiflik_durumu = 0 ;
    ork->Kara_Troll.etki_degeri = 0 ;
    strcpy(ork->Kara_Troll.etki_turu, " ");
    strcpy(ork->Kara_Troll.aciklama, " ");

    ork->Golge_Kurtlari.aktiflik_durumu = 0 ;
    ork->Golge_Kurtlari.etki_degeri = 0 ;
    strcpy(ork->Golge_Kurtlari.etki_turu, " ");
    strcpy(ork->Golge_Kurtlari.aciklama, " ");

    ork->Camur_Devleri.aktiflik_durumu = 0 ;
    ork->Camur_Devleri.etki_degeri = 0 ;
    strcpy(ork->Camur_Devleri.etki_turu, " ");
    strcpy(ork->Camur_Devleri.aciklama, " ");

    ork->Ates_Iblisi.aktiflik_durumu = 0 ;
    ork->Ates_Iblisi.etki_degeri = 0 ;
    strcpy(ork->Ates_Iblisi.etki_turu, " ");
    strcpy(ork->Ates_Iblisi.aciklama, " ");

    ork->Makrog_Savas_Beyi.aktiflik_durumu = 0 ;
    ork->Makrog_Savas_Beyi.etki_degeri = 0 ;
    strcpy(ork->Makrog_Savas_Beyi.etki_turu, " ");
    strcpy(ork->Makrog_Savas_Beyi.aciklama, " ");

    ork->Buz_Devleri.aktiflik_durumu = 0 ;
    ork->Buz_Devleri.etki_degeri = 0 ;
    strcpy(ork->Buz_Devleri.etki_turu, " ");
    strcpy(ork->Buz_Devleri.aciklama, " ");

    insan->insan_arastirmalari.savunma_ustaligi.arturu_aktiflik = 0 ;
    insan->insan_arastirmalari.saldiri_gelistirmesi.arturu_aktiflik = 0 ;
    insan->insan_arastirmalari.elit_egitim.arturu_aktiflik = 0 ;
    insan->insan_arastirmalari.kusatma_ustaligi.arturu_aktiflik = 0 ;

    insan->insan_arastirmalari.savunma_ustaligi.seviye1.deger = 0 ;
    insan->insan_arastirmalari.savunma_ustaligi.seviye1.aktiflik_durumu = 0 ;
    strcpy(insan->insan_arastirmalari.savunma_ustaligi.seviye1.aciklama, " ");

    insan->insan_arastirmalari.savunma_ustaligi.seviye2.deger = 0 ;
    insan->insan_arastirmalari.savunma_ustaligi.seviye2.aktiflik_durumu = 0 ;
    strcpy(insan->insan_arastirmalari.savunma_ustaligi.seviye2.aciklama, " ");

    insan->insan_arastirmalari.savunma_ustaligi.seviye3.deger = 0 ;
    insan->insan_arastirmalari.savunma_ustaligi.seviye3.aktiflik_durumu = 0 ;
    strcpy(insan->insan_arastirmalari.savunma_ustaligi.seviye3.aciklama, " ");

    insan->insan_arastirmalari.saldiri_gelistirmesi.seviye1.deger = 0 ;
    insan->insan_arastirmalari.saldiri_gelistirmesi.seviye1.aktiflik_durumu = 0 ;
    strcpy(insan->insan_arastirmalari.saldiri_gelistirmesi.seviye1.aciklama, " ");

    insan->insan_arastirmalari.saldiri_gelistirmesi.seviye2.deger = 0 ;
    insan->insan_arastirmalari.saldiri_gelistirmesi.seviye2.aktiflik_durumu = 0 ;
    strcpy(insan->insan_arastirmalari.saldiri_gelistirmesi.seviye2.aciklama, " ");

    insan->insan_arastirmalari.saldiri_gelistirmesi.seviye3.deger = 0 ;
    insan->insan_arastirmalari.saldiri_gelistirmesi.seviye3.aktiflik_durumu = 0 ;
    strcpy(insan->insan_arastirmalari.saldiri_gelistirmesi.seviye3.aciklama, " ");

    insan->insan_arastirmalari.elit_egitim.seviye1.deger = 0 ;
    insan->insan_arastirmalari.elit_egitim.seviye1.aktiflik_durumu = 0 ;
    strcpy(insan->insan_arastirmalari.elit_egitim.seviye1.aciklama, " ");

    insan->insan_arastirmalari.elit_egitim.seviye2.deger = 0 ;
    insan->insan_arastirmalari.elit_egitim.seviye2.aktiflik_durumu = 0 ;
    strcpy(insan->insan_arastirmalari.elit_egitim.seviye2.aciklama, " ");

    insan->insan_arastirmalari.elit_egitim.seviye3.deger = 0 ;
    insan->insan_arastirmalari.elit_egitim.seviye3.aktiflik_durumu = 0 ;
    strcpy(insan->insan_arastirmalari.elit_egitim.seviye3.aciklama, " ");

    insan->insan_arastirmalari.kusatma_ustaligi.seviye1.deger = 0 ;
    insan->insan_arastirmalari.kusatma_ustaligi.seviye1.aktiflik_durumu = 0 ;
    strcpy(insan->insan_arastirmalari.kusatma_ustaligi.seviye1.aciklama, " ");

    insan->insan_arastirmalari.kusatma_ustaligi.seviye2.deger = 0 ;
    insan->insan_arastirmalari.kusatma_ustaligi.seviye2.aktiflik_durumu = 0 ;
    strcpy(insan->insan_arastirmalari.kusatma_ustaligi.seviye2.aciklama, " ");

    insan->insan_arastirmalari.kusatma_ustaligi.seviye3.deger = 0 ;
    insan->insan_arastirmalari.kusatma_ustaligi.seviye3.aktiflik_durumu = 0 ;
    strcpy(insan->insan_arastirmalari.kusatma_ustaligi.seviye3.aciklama, " ");

    ork->ork_arastirmalari.savunma_ustaligi.arturu_aktiflik = 0 ;
    ork->ork_arastirmalari.saldiri_gelistirmesi.arturu_aktiflik = 0 ;
    ork->ork_arastirmalari.elit_egitim.arturu_aktiflik = 0 ;
    ork->ork_arastirmalari.kusatma_ustaligi.arturu_aktiflik = 0 ;


    ork->ork_arastirmalari.savunma_ustaligi.arturu_aktiflik = 0 ;
    ork->ork_arastirmalari.saldiri_gelistirmesi.arturu_aktiflik = 0 ;
    ork->ork_arastirmalari.elit_egitim.arturu_aktiflik = 0 ;
    ork->ork_arastirmalari.kusatma_ustaligi.arturu_aktiflik = 0 ;

    ork->ork_arastirmalari.savunma_ustaligi.seviye1.deger = 0 ;
    ork->ork_arastirmalari.savunma_ustaligi.seviye1.aktiflik_durumu = 0 ;
    strcpy(ork->ork_arastirmalari.savunma_ustaligi.seviye1.aciklama, " ");

    ork->ork_arastirmalari.savunma_ustaligi.seviye2.deger = 0 ;
    ork->ork_arastirmalari.savunma_ustaligi.seviye2.aktiflik_durumu = 0 ;
    strcpy(ork->ork_arastirmalari.savunma_ustaligi.seviye2.aciklama, " ");

    ork->ork_arastirmalari.savunma_ustaligi.seviye3.deger = 0 ;
    ork->ork_arastirmalari.savunma_ustaligi.seviye3.aktiflik_durumu = 0 ;
    strcpy(ork->ork_arastirmalari.savunma_ustaligi.seviye3.aciklama, " ");

    ork->ork_arastirmalari.saldiri_gelistirmesi.seviye1.deger = 0 ;
    ork->ork_arastirmalari.saldiri_gelistirmesi.seviye1.aktiflik_durumu = 0 ;
    strcpy(ork->ork_arastirmalari.saldiri_gelistirmesi.seviye1.aciklama, " ");

    ork->ork_arastirmalari.saldiri_gelistirmesi.seviye2.deger = 0 ;
    ork->ork_arastirmalari.saldiri_gelistirmesi.seviye2.aktiflik_durumu = 0 ;
    strcpy(ork->ork_arastirmalari.saldiri_gelistirmesi.seviye2.aciklama, " ");

    ork->ork_arastirmalari.saldiri_gelistirmesi.seviye3.deger = 0 ;
    ork->ork_arastirmalari.saldiri_gelistirmesi.seviye3.aktiflik_durumu = 0 ;
    strcpy(ork->ork_arastirmalari.saldiri_gelistirmesi.seviye3.aciklama, " ");

    ork->ork_arastirmalari.elit_egitim.seviye1.deger = 0 ;
    ork->ork_arastirmalari.elit_egitim.seviye1.aktiflik_durumu = 0 ;
    strcpy(ork->ork_arastirmalari.elit_egitim.seviye1.aciklama, " ");

    ork->ork_arastirmalari.elit_egitim.seviye2.deger = 0 ;
    ork->ork_arastirmalari.elit_egitim.seviye2.aktiflik_durumu = 0 ;
    strcpy(ork->ork_arastirmalari.elit_egitim.seviye2.aciklama, " ");

    ork->ork_arastirmalari.elit_egitim.seviye3.deger = 0 ;
    ork->ork_arastirmalari.elit_egitim.seviye3.aktiflik_durumu = 0 ;
    strcpy(ork->ork_arastirmalari.elit_egitim.seviye3.aciklama, " ");

    ork->ork_arastirmalari.kusatma_ustaligi.seviye1.deger = 0 ;
    ork->ork_arastirmalari.kusatma_ustaligi.seviye1.aktiflik_durumu = 0 ;
    strcpy(ork->ork_arastirmalari.kusatma_ustaligi.seviye1.aciklama, " ");

    ork->ork_arastirmalari.kusatma_ustaligi.seviye2.deger = 0 ;
    ork->ork_arastirmalari.kusatma_ustaligi.seviye2.aktiflik_durumu = 0 ;
    strcpy(ork->ork_arastirmalari.kusatma_ustaligi.seviye2.aciklama, " ");

    ork->ork_arastirmalari.kusatma_ustaligi.seviye3.deger = 0 ;
    ork->ork_arastirmalari.kusatma_ustaligi.seviye3.aktiflik_durumu = 0 ;
    strcpy(ork->ork_arastirmalari.kusatma_ustaligi.seviye3.aciklama, " ");

    insan->piyadeler.sayi = 0 ;
    insan->okcular.sayi = 0 ;
    insan->suvariler.sayi = 0 ;
    insan->kusatma_makineleri.sayi = 0 ;
    ork->ork_dovusculeri.sayi = 0 ;
    ork->mizrakcilar.sayi = 0 ;
    ork->varg_binicileri.sayi = 0 ;
    ork->troller.sayi = 0 ;





}

void savas_ilk_deger_atama(struct taraf1 *insan_tarafi, struct taraf2 *ork_tarafi) {

    insan_tarafi->piyadeler.guncel_saldiri_gucu = insan_tarafi->piyadeler.saldiri_gucu;
    insan_tarafi->piyadeler.guncel_savunma_gucu = insan_tarafi->piyadeler.savunma_gucu;
    insan_tarafi->okcular.guncel_saldiri_gucu = insan_tarafi->okcular.saldiri_gucu;
    insan_tarafi->okcular.guncel_savunma_gucu = insan_tarafi->okcular.savunma_gucu;
    insan_tarafi->suvariler.guncel_saldiri_gucu = insan_tarafi->suvariler.saldiri_gucu;
    insan_tarafi->suvariler.guncel_savunma_gucu = insan_tarafi->suvariler.savunma_gucu;
    insan_tarafi->kusatma_makineleri.guncel_saldiri_gucu = insan_tarafi->kusatma_makineleri.saldiri_gucu;
    insan_tarafi->kusatma_makineleri.guncel_savunma_gucu = insan_tarafi->kusatma_makineleri.savunma_gucu;

    ork_tarafi->ork_dovusculeri.guncel_saldiri_gucu = ork_tarafi->ork_dovusculeri.saldiri_gucu;
    ork_tarafi->ork_dovusculeri.guncel_savunma_gucu = ork_tarafi->ork_dovusculeri.savunma_gucu;
    ork_tarafi->varg_binicileri.guncel_saldiri_gucu = ork_tarafi->varg_binicileri.saldiri_gucu;
    ork_tarafi->varg_binicileri.guncel_savunma_gucu = ork_tarafi->varg_binicileri.savunma_gucu;
    ork_tarafi->mizrakcilar.guncel_saldiri_gucu = ork_tarafi->mizrakcilar.saldiri_gucu;
    ork_tarafi->mizrakcilar.guncel_savunma_gucu = ork_tarafi->mizrakcilar.savunma_gucu;
    ork_tarafi->troller.guncel_saldiri_gucu = ork_tarafi->troller.saldiri_gucu;
    ork_tarafi->troller.guncel_savunma_gucu = ork_tarafi->troller.savunma_gucu;

}

void birimleri_ayristir(FILE *kavga,struct taraf1 *insan_lejyonu,struct taraf2 *ork_lejyonu ){


    char dizi[256];
    char deger_atama[256];

    while(fgets(dizi,sizeof(dizi),kavga)!=NULL)
    {
        if(strstr(dizi,"\"piyadeler\""))
        {

            while(fgets(deger_atama,sizeof(deger_atama),kavga)!=NULL)
            {

                if(strstr(deger_atama,"\"saldiri\""))
                {
                    sscanf(strstr(deger_atama,"\"saldiri\":") + 10,"%d",&insan_lejyonu->piyadeler.saldiri_gucu);

                }
                else if (strstr(deger_atama,"\"savunma\""))
                {

                    sscanf(strstr(deger_atama,"\"savunma\":") + 11,"%d",&insan_lejyonu->piyadeler.savunma_gucu);

                }
                else if (strstr(deger_atama,"\"saglik\""))
                {

                    sscanf(strstr(deger_atama,"\"saglik\":") + 9,"%d",&insan_lejyonu->piyadeler.saglik);

                }
                else if (strstr(deger_atama,"\"kritik_sans\""))
                {

                    sscanf(strstr(deger_atama,"\"kritik_sans\":") + 15,"%lf",&insan_lejyonu->piyadeler.kritik_sans);
                }

                if(strstr(deger_atama,"}"))
                {

                    break  ;
                }
            }
        }
        if(strstr(dizi,"\"okcular\""))
        {

            while(fgets(deger_atama,sizeof(deger_atama),kavga)!=NULL)
            {

                if(strstr(deger_atama,"\"saldiri\""))
                {
                    sscanf(strstr(deger_atama,"\"saldiri\":") + 10,"%d",&insan_lejyonu->okcular.saldiri_gucu);

                }
                else if (strstr(deger_atama,"\"savunma\""))
                {

                    sscanf(strstr(deger_atama,"\"savunma\":") + 11,"%d",&insan_lejyonu->okcular.savunma_gucu);

                }
                else if (strstr(deger_atama,"\"saglik\""))
                {

                    sscanf(strstr(deger_atama,"\"saglik\":") + 9,"%d",&insan_lejyonu->okcular.saglik);

                }
                else if (strstr(deger_atama,"\"kritik_sans\""))
                {

                    sscanf(strstr(deger_atama,"\"kritik_sans\":") + 15,"%lf",&insan_lejyonu->okcular.kritik_sans);
                }

                if(strstr(deger_atama,"}"))
                {

                    break  ;
                }
            }
        }

        if(strstr(dizi,"\"suvariler\""))
        {

            while(fgets(deger_atama,sizeof(deger_atama),kavga)!=NULL)
            {

                if(strstr(deger_atama,"\"saldiri\""))
                {
                    sscanf(strstr(deger_atama,"\"saldiri\":") + 10,"%d",&insan_lejyonu->suvariler.saldiri_gucu);

                }
                else if (strstr(deger_atama,"\"savunma\""))
                {

                    sscanf(strstr(deger_atama,"\"savunma\":") + 11,"%d",&insan_lejyonu->suvariler.savunma_gucu);

                }
                else if (strstr(deger_atama,"\"saglik\""))
                {

                    sscanf(strstr(deger_atama,"\"saglik\":") + 9,"%d",&insan_lejyonu->suvariler.saglik);

                }
                else if (strstr(deger_atama,"\"kritik_sans\""))
                {

                    sscanf(strstr(deger_atama,"\"kritik_sans\":") + 15,"%lf",&insan_lejyonu->suvariler.kritik_sans);
                }

                if(strstr(deger_atama,"}"))
                {

                    break  ;
                }
            }
        }

        if(strstr(dizi,"\"kusatma_makineleri\""))
        {

            while(fgets(deger_atama,sizeof(deger_atama),kavga)!=NULL)
            {

                if(strstr(deger_atama,"\"saldiri\""))
                {
                    sscanf(strstr(deger_atama,"\"saldiri\":") + 10,"%d",&insan_lejyonu->kusatma_makineleri.saldiri_gucu);

                }
                else if (strstr(deger_atama,"\"savunma\""))
                {

                    sscanf(strstr(deger_atama,"\"savunma\":") + 11,"%d",&insan_lejyonu->kusatma_makineleri.savunma_gucu);

                }
                else if (strstr(deger_atama,"\"saglik\""))
                {

                    sscanf(strstr(deger_atama,"\"saglik\":") + 9,"%d",&insan_lejyonu->kusatma_makineleri.saglik);

                }
                else if (strstr(deger_atama,"\"kritik_sans\""))
                {

                    sscanf(strstr(deger_atama,"\"kritik_sans\":") + 15,"%lf",&insan_lejyonu->kusatma_makineleri.kritik_sans);
                }

                if(strstr(deger_atama,"}"))
                {

                    break  ;
                }
            }
        }

        if(strstr(dizi,"\"ork_dovusculeri\""))
        {

            while(fgets(deger_atama,sizeof(deger_atama),kavga)!=NULL)
            {

                if(strstr(deger_atama,"\"saldiri\""))
                {
                    sscanf(strstr(deger_atama,"\"saldiri\":") + 10,"%d",&ork_lejyonu->ork_dovusculeri.saldiri_gucu);

                }
                else if (strstr(deger_atama,"\"savunma\""))
                {

                    sscanf(strstr(deger_atama,"\"savunma\":") + 11,"%d",&ork_lejyonu->ork_dovusculeri.savunma_gucu);

                }
                else if (strstr(deger_atama,"\"saglik\""))
                {

                    sscanf(strstr(deger_atama,"\"saglik\":") + 9,"%d",&ork_lejyonu->ork_dovusculeri.saglik);

                }
                else if (strstr(deger_atama,"\"kritik_sans\""))
                {

                    sscanf(strstr(deger_atama,"\"kritik_sans\":") + 15,"%lf",&ork_lejyonu->ork_dovusculeri.kritik_sans);
                }

                if(strstr(deger_atama,"}"))
                {

                    break  ;
                }
            }
        }

        if(strstr(dizi,"\"mizrakcilar\""))
        {

            while(fgets(deger_atama,sizeof(deger_atama),kavga)!=NULL)
            {

                if(strstr(deger_atama,"\"saldiri\""))
                {
                    sscanf(strstr(deger_atama,"\"saldiri\":") + 10,"%d",&ork_lejyonu->mizrakcilar.saldiri_gucu);

                }
                else if (strstr(deger_atama,"\"savunma\""))
                {

                    sscanf(strstr(deger_atama,"\"savunma\":") + 11,"%d",&ork_lejyonu->mizrakcilar.savunma_gucu);

                }
                else if (strstr(deger_atama,"\"saglik\""))
                {

                    sscanf(strstr(deger_atama,"\"saglik\":") + 9,"%d",&ork_lejyonu->mizrakcilar.saglik);

                }
                else if (strstr(deger_atama,"\"kritik_sans\""))
                {

                    sscanf(strstr(deger_atama,"\"kritik_sans\":") + 15,"%lf",&ork_lejyonu->mizrakcilar.kritik_sans);
                }

                if(strstr(deger_atama,"}"))
                {

                    break  ;
                }
            }
        }
        if(strstr(dizi,"\"varg_binicileri\""))
        {

            while(fgets(deger_atama,sizeof(deger_atama),kavga)!=NULL)
            {

                if(strstr(deger_atama,"\"saldiri\""))
                {
                    sscanf(strstr(deger_atama,"\"saldiri\":") + 10,"%d",&ork_lejyonu->varg_binicileri.saldiri_gucu);

                }
                else if (strstr(deger_atama,"\"savunma\""))
                {

                    sscanf(strstr(deger_atama,"\"savunma\":") + 11,"%d",&ork_lejyonu->varg_binicileri.savunma_gucu);

                }
                else if (strstr(deger_atama,"\"saglik\""))
                {

                    sscanf(strstr(deger_atama,"\"saglik\":") + 9,"%d",&ork_lejyonu->varg_binicileri.saglik);

                }
                else if (strstr(deger_atama,"\"kritik_sans\""))
                {

                    sscanf(strstr(deger_atama,"\"kritik_sans\":") + 15,"%lf",&ork_lejyonu->varg_binicileri.kritik_sans);
                }

                if(strstr(deger_atama,"}"))
                {

                    break  ;
                }
            }
        }


        if(strstr(dizi,"\"troller\""))
        {

            while(fgets(deger_atama,sizeof(deger_atama),kavga)!=NULL)
            {

                if(strstr(deger_atama,"\"saldiri\""))
                {
                    sscanf(strstr(deger_atama,"\"saldiri\":") + 10,"%d",&ork_lejyonu->troller.saldiri_gucu);

                }
                else if (strstr(deger_atama,"\"savunma\""))
                {

                    sscanf(strstr(deger_atama,"\"savunma\":") + 11,"%d",&ork_lejyonu->troller.savunma_gucu);

                }
                else if (strstr(deger_atama,"\"saglik\""))
                {

                    sscanf(strstr(deger_atama,"\"saglik\":") + 9,"%d",&ork_lejyonu->troller.saglik);

                }
                else if (strstr(deger_atama,"\"kritik_sans\""))
                {

                    sscanf(strstr(deger_atama,"\"kritik_sans\":") + 15,"%lf",&ork_lejyonu->troller.kritik_sans);
                }

                if(strstr(deger_atama,"}"))
                {

                    break  ;
                }
            }
        }

    }
}

void heros_deger_atama(FILE *fp,struct taraf1 *insan,struct taraf2 *orklar){


    char dizi[256];
    char deger_atama[256];

    while(fgets(dizi,sizeof(dizi),fp)!=NULL)
    {
        if(strstr(dizi,"\"Alparslan\""))
        {

            while(fgets(deger_atama,sizeof(deger_atama),fp)!=NULL)
            {

                if(strstr(deger_atama,"\"bonus_turu\""))
                {
                    sscanf(deger_atama, " \"bonus_turu\": \"%[^\"]\",", insan->Alparslan.bonus_turu);
                }
                else if (strstr(deger_atama,"\"bonus_degeri\""))
                {

                    sscanf(strstr(deger_atama,"\"bonus_degeri\":") + 16,"\"%d\"",&insan->Alparslan.bonus_degeri);

                }
                else if (strstr(deger_atama,"\"aciklama\""))
                {

                    sscanf(deger_atama, " \"aciklama\": \"%[^\"]\",", insan->Alparslan.aciklama);
                }


                if(strstr(deger_atama,"}"))
                {

                    break  ;
                }
            }
        }
     if(strstr(dizi,"\"Mete_han\""))
        {

            while(fgets(deger_atama,sizeof(deger_atama),fp)!=NULL)
            {

                if(strstr(deger_atama,"\"bonus_turu\""))
                {
                    sscanf(deger_atama, " \"bonus_turu\": \"%[^\"]\",", insan->Mete_Han.bonus_turu);
                }
                else if (strstr(deger_atama,"\"bonus_degeri\""))
                {

                    sscanf(strstr(deger_atama,"\"bonus_degeri\":") + 16,"\"%d\"",&insan->Mete_Han.bonus_degeri);

                }
                else if (strstr(deger_atama,"\"aciklama\""))
                {

                    sscanf(deger_atama, " \"aciklama\": \"%[^\"]\",", insan->Mete_Han.aciklama);
                }


                if(strstr(deger_atama,"}"))
                {

                    break  ;
                }
            }
        }
        if(strstr(dizi,"\"Fatih_Sultan_Mehmet\""))
        {

            while(fgets(deger_atama,sizeof(deger_atama),fp)!=NULL)
            {

                if(strstr(deger_atama,"\"bonus_turu\""))
                {
                    sscanf(deger_atama, " \"bonus_turu\": \"%[^\"]\",", insan->Fatih_Sultan_Mehmet.bonus_turu);
                }
                else if (strstr(deger_atama,"\"bonus_degeri\""))
                {

                    sscanf(strstr(deger_atama,"\"bonus_degeri\":") + 16,"\"%d\"",&insan->Fatih_Sultan_Mehmet.bonus_degeri);

                }
                else if (strstr(deger_atama,"\"aciklama\""))
                {

                    sscanf(deger_atama, " \"aciklama\": \"%[^\"]\",", insan->Fatih_Sultan_Mehmet.aciklama);
                }


                if(strstr(deger_atama,"}"))
                {

                    break  ;
                }
            }
        }
        if(strstr(dizi,"\"Yavuz_Sultan_Selim\""))
        {

            while(fgets(deger_atama,sizeof(deger_atama),fp)!=NULL)
            {

                if(strstr(deger_atama,"\"bonus_turu\""))
                {
                    sscanf(deger_atama, " \"bonus_turu\": \"%[^\"]\",", insan->Yavuz_Sultan_Selim.bonus_turu);
                }
                else if (strstr(deger_atama,"\"bonus_degeri\""))
                {

                    sscanf(strstr(deger_atama,"\"bonus_degeri\":") + 16,"\"%d\"",&insan->Yavuz_Sultan_Selim.bonus_degeri);

                }
                else if (strstr(deger_atama,"\"aciklama\""))
                {

                    sscanf(deger_atama, " \"aciklama\": \"%[^\"]\",", insan->Yavuz_Sultan_Selim.aciklama);
                }


                if(strstr(deger_atama,"}"))
                {

                    break  ;
                }
            }
        }
        if(strstr(dizi,"\"Tugrul_Bey\""))
        {

            while(fgets(deger_atama,sizeof(deger_atama),fp)!=NULL)
            {

                if(strstr(deger_atama,"\"bonus_turu\""))
                {
                    sscanf(deger_atama, " \"bonus_turu\": \"%[^\"]\",", insan->Tugrul_Bey.bonus_turu);
                }
                else if (strstr(deger_atama,"\"bonus_degeri\""))
                {

                    sscanf(strstr(deger_atama,"\"bonus_degeri\":") + 16,"\"%d\"",&insan->Tugrul_Bey.bonus_degeri);

                }
                else if (strstr(deger_atama,"\"aciklama\""))
                {

                    sscanf(deger_atama, " \"aciklama\": \"%[^\"]\",", insan->Tugrul_Bey.aciklama);
                }


                if(strstr(deger_atama,"}"))
                {

                    break  ;
                }
            }
        }
        if(strstr(dizi,"\"Goruk_Vahsi\""))
        {

            while(fgets(deger_atama,sizeof(deger_atama),fp)!=NULL)
            {

                if(strstr(deger_atama,"\"bonus_turu\""))
                {
                    sscanf(deger_atama, " \"bonus_turu\": \"%[^\"]\",", orklar->Goruk_Vahsi.bonus_turu);
                }
                else if (strstr(deger_atama,"\"bonus_degeri\""))
                {

                    sscanf(strstr(deger_atama,"\"bonus_degeri\":") + 16,"\"%d\"",&orklar->Goruk_Vahsi.bonus_degeri);

                }
                else if (strstr(deger_atama,"\"aciklama\""))
                {

                    sscanf(deger_atama, " \"aciklama\": \"%[^\"]\",", orklar->Goruk_Vahsi.aciklama);
                }


                if(strstr(deger_atama,"}"))
                {

                    break  ;
                }
            }
        }

        if(strstr(dizi,"\"Thruk_Kemikkiran\""))
        {

            while(fgets(deger_atama,sizeof(deger_atama),fp)!=NULL)
            {

                if(strstr(deger_atama,"\"bonus_turu\""))
                {
                    sscanf(deger_atama, " \"bonus_turu\": \"%[^\"]\",", orklar->Thruk_Kemikkiran.bonus_turu);
                }
                else if (strstr(deger_atama,"\"bonus_degeri\""))
                {

                    sscanf(strstr(deger_atama,"\"bonus_degeri\":") + 16,"\"%d\"",&orklar->Thruk_Kemikkiran.bonus_degeri);

                }
                else if (strstr(deger_atama,"\"aciklama\""))
                {

                    sscanf(deger_atama, " \"aciklama\": \"%[^\"]\",", orklar->Thruk_Kemikkiran.aciklama);
                }


                if(strstr(deger_atama,"}"))
                {

                    break  ;
                }
            }
        }
        if(strstr(dizi,"\"Vrog_Kafakiran\""))
        {

            while(fgets(deger_atama,sizeof(deger_atama),fp)!=NULL)
            {

                if(strstr(deger_atama,"\"bonus_turu\""))
                {
                    sscanf(deger_atama, " \"bonus_turu\": \"%[^\"]\",", orklar->Vrog_Kafakiran.bonus_turu);
                }
                else if (strstr(deger_atama,"\"bonus_degeri\""))
                {

                    sscanf(strstr(deger_atama,"\"bonus_degeri\":") + 16,"\"%d\"",&orklar->Vrog_Kafakiran.bonus_degeri);

                }
                else if (strstr(deger_atama,"\"aciklama\""))
                {

                    sscanf(deger_atama, " \"aciklama\": \"%[^\"]\",", orklar->Vrog_Kafakiran.aciklama);
                }


                if(strstr(deger_atama,"}"))
                {

                    break  ;
                }
            }
        }
         if(strstr(dizi,"\"Ugar_Zalim\""))
        {

            while(fgets(deger_atama,sizeof(deger_atama),fp)!=NULL)
            {

                if(strstr(deger_atama,"\"bonus_turu\""))
                {
                    sscanf(deger_atama, " \"bonus_turu\": \"%[^\"]\",", orklar->Ugar_Zalim.bonus_turu);
                }
                else if (strstr(deger_atama,"\"bonus_degeri\""))
                {

                    sscanf(strstr(deger_atama,"\"bonus_degeri\":") + 16,"\"%d\"",&orklar->Ugar_Zalim.bonus_degeri);

                }
                else if (strstr(deger_atama,"\"aciklama\""))
                {

                    sscanf(deger_atama, " \"aciklama\": \"%[^\"]\",", orklar->Ugar_Zalim.aciklama);
                }


                if(strstr(deger_atama,"}"))
                {

                    break  ;
                }
            }
        }


}



}

void creatures_deger_atama(FILE *fp1,struct taraf1 *insan,struct taraf2 *ork){

    char dizi[256];
    char deger_atama[256];

    while(fgets(dizi,sizeof(dizi),fp1)!=NULL)
    {
        if(strstr(dizi,"\"Ejderha\""))
        {

            while(fgets(deger_atama,sizeof(deger_atama),fp1)!=NULL)
            {
                if (strstr(deger_atama,"\"etki_degeri\""))
                {
                    sscanf(strstr(deger_atama,"\"etki_degeri\":") + 15,"\"%d\"",&insan->Ejderha.etki_degeri);
                }
                else if(strstr(deger_atama,"\"etki_turu\""))
                {
                    sscanf(deger_atama, " \"etki_turu\": \"%[^\"]\",", insan->Ejderha.etki_turu);
                }

                else if (strstr(deger_atama,"\"aciklama\""))
                {

                    sscanf(deger_atama, " \"aciklama\": \"%[^\"]\",", insan->Ejderha.aciklama);
                }


                if(strstr(deger_atama,"}"))
                {

                    break  ;
                }
            }
        }
        if(strstr(dizi,"\"Agri_Dagi_Devleri\""))
        {

            while(fgets(deger_atama,sizeof(deger_atama),fp1)!=NULL)
            {
                if (strstr(deger_atama,"\"etki_degeri\""))
                {
                    sscanf(strstr(deger_atama,"\"etki_degeri\":") + 15,"\"%d\"",&insan->Agri_Dagi_Devleri.etki_degeri);
                }
                else if(strstr(deger_atama,"\"etki_turu\""))
                {
                    sscanf(deger_atama, " \"etki_turu\": \"%[^\"]\",", insan->Agri_Dagi_Devleri.etki_turu);
                }

                else if (strstr(deger_atama,"\"aciklama\""))
                {

                    sscanf(deger_atama, " \"aciklama\": \"%[^\"]\",", insan->Agri_Dagi_Devleri.aciklama);
                }


                if(strstr(deger_atama,"}"))
                {

                    break  ;
                }
            }
        }
        if(strstr(dizi,"\"Tepegoz\""))
        {

            while(fgets(deger_atama,sizeof(deger_atama),fp1)!=NULL)
            {
                if (strstr(deger_atama,"\"etki_degeri\""))
                {
                    sscanf(strstr(deger_atama,"\"etki_degeri\":") + 15,"\"%d\"",&insan->Tepegoz.etki_degeri);
                }
                else if(strstr(deger_atama,"\"etki_turu\""))
                {
                    sscanf(deger_atama, " \"etki_turu\": \"%[^\"]\",", insan->Tepegoz.etki_turu);
                }

                else if (strstr(deger_atama,"\"aciklama\""))
                {

                    sscanf(deger_atama, " \"aciklama\": \"%[^\"]\",", insan->Tepegoz.aciklama);
                }


                if(strstr(deger_atama,"}"))
                {

                    break  ;
                }
            }
        }
        if(strstr(dizi,"\"Karakurt\""))
        {

            while(fgets(deger_atama,sizeof(deger_atama),fp1)!=NULL)
            {
                if (strstr(deger_atama,"\"etki_degeri\""))
                {
                    sscanf(strstr(deger_atama,"\"etki_degeri\":") + 15,"\"%d\"",&insan->Karakurt.etki_degeri);
                }
                else if(strstr(deger_atama,"\"etki_turu\""))
                {
                    sscanf(deger_atama, " \"etki_turu\": \"%[^\"]\",", insan->Karakurt.etki_turu);
                }

                else if (strstr(deger_atama,"\"aciklama\""))
                {

                    sscanf(deger_atama, " \"aciklama\": \"%[^\"]\",", insan->Karakurt.aciklama);
                }


                if(strstr(deger_atama,"}"))
                {

                    break  ;
                }
            }
        }
        if(strstr(dizi,"\"Samur\""))
        {

            while(fgets(deger_atama,sizeof(deger_atama),fp1)!=NULL)
            {
                if (strstr(deger_atama,"\"etki_degeri\""))
                {
                    sscanf(strstr(deger_atama,"\"etki_degeri\":") + 15,"\"%d\"",&insan->Samur.etki_degeri);
                }
                else if(strstr(deger_atama,"\"etki_turu\""))
                {
                    sscanf(deger_atama, " \"etki_turu\": \"%[^\"]\",", insan->Samur.etki_turu);
                }

                else if (strstr(deger_atama,"\"aciklama\""))
                {

                    sscanf(deger_atama, " \"aciklama\": \"%[^\"]\",", insan->Samur.aciklama);
                }


                if(strstr(deger_atama,"}"))
                {

                    break  ;
                }
            }
        }
        if(strstr(dizi,"\"Kara_Troll\""))
        {

            while(fgets(deger_atama,sizeof(deger_atama),fp1)!=NULL)
            {
                if (strstr(deger_atama,"\"etki_degeri\""))
                {
                    sscanf(strstr(deger_atama,"\"etki_degeri\":") + 15,"\"%d\"",&ork->Kara_Troll.etki_degeri);
                }
                else if(strstr(deger_atama,"\"etki_turu\""))
                {
                    sscanf(deger_atama, " \"etki_turu\": \"%[^\"]\",", ork->Kara_Troll.etki_turu);
                }

                else if (strstr(deger_atama,"\"aciklama\""))
                {

                    sscanf(deger_atama, " \"aciklama\": \"%[^\"]\",", ork->Kara_Troll.aciklama);
                }


                if(strstr(deger_atama,"}"))
                {

                    break  ;
                }
            }
        }
        if(strstr(dizi,"\"Makrog_Savas_Beyi\""))
        {

            while(fgets(deger_atama,sizeof(deger_atama),fp1)!=NULL)
            {
                if (strstr(deger_atama,"\"etki_degeri\""))
                {
                    sscanf(strstr(deger_atama,"\"etki_degeri\":") + 15,"\"%d\"",&ork->Makrog_Savas_Beyi.etki_degeri);
                }
                else if(strstr(deger_atama,"\"etki_turu\""))
                {
                    sscanf(deger_atama, " \"etki_turu\": \"%[^\"]\",", ork->Makrog_Savas_Beyi.etki_turu);
                }

                else if (strstr(deger_atama,"\"aciklama\""))
                {

                    sscanf(deger_atama, " \"aciklama\": \"%[^\"]\",", ork->Makrog_Savas_Beyi.aciklama);
                }


                if(strstr(deger_atama,"}"))
                {

                    break  ;
                }
            }
        }
        if(strstr(dizi,"\"Golge_Kurtlari\""))
        {

            while(fgets(deger_atama,sizeof(deger_atama),fp1)!=NULL)
            {
                if (strstr(deger_atama,"\"etki_degeri\""))
                {
                    sscanf(strstr(deger_atama,"\"etki_degeri\":") + 15,"\"%d\"",&ork->Golge_Kurtlari.etki_degeri);
                }
                else if(strstr(deger_atama,"\"etki_turu\""))
                {
                    sscanf(deger_atama, " \"etki_turu\": \"%[^\"]\",", ork->Golge_Kurtlari.etki_turu);
                }

                else if (strstr(deger_atama,"\"aciklama\""))
                {

                    sscanf(deger_atama, " \"aciklama\": \"%[^\"]\",", ork->Golge_Kurtlari.aciklama);
                }


                if(strstr(deger_atama,"}"))
                {

                    break  ;
                }
            }
        }
        if(strstr(dizi,"\"Camur_Devleri\""))
        {

            while(fgets(deger_atama,sizeof(deger_atama),fp1)!=NULL)
            {
                if (strstr(deger_atama,"\"etki_degeri\""))
                {
                    sscanf(strstr(deger_atama,"\"etki_degeri\":") + 15,"\"%d\"",&ork->Camur_Devleri.etki_degeri);
                }
                else if(strstr(deger_atama,"\"etki_turu\""))
                {
                    sscanf(deger_atama, " \"etki_turu\": \"%[^\"]\",", ork->Camur_Devleri.etki_turu);
                }

                else if (strstr(deger_atama,"\"aciklama\""))
                {

                    sscanf(deger_atama, " \"aciklama\": \"%[^\"]\",", ork->Camur_Devleri.aciklama);
                }


                if(strstr(deger_atama,"}"))
                {

                    break  ;
                }
            }
        }
        if(strstr(dizi,"\"Ates_Iblisi\""))
        {

            while(fgets(deger_atama,sizeof(deger_atama),fp1)!=NULL)
            {
                if (strstr(deger_atama,"\"etki_degeri\""))
                {
                    sscanf(strstr(deger_atama,"\"etki_degeri\":") + 15,"\"%d\"",&ork->Ates_Iblisi.etki_degeri);
                }
                else if(strstr(deger_atama,"\"etki_turu\""))
                {
                    sscanf(deger_atama, " \"etki_turu\": \"%[^\"]\",", ork->Ates_Iblisi.etki_turu);
                }

                else if (strstr(deger_atama,"\"aciklama\""))
                {

                    sscanf(deger_atama, " \"aciklama\": \"%[^\"]\",", ork->Ates_Iblisi.aciklama);
                }


                if(strstr(deger_atama,"}"))
                {

                    break  ;
                }
            }
        }
        if(strstr(dizi,"\"Buz_Devleri\""))
        {

            while(fgets(deger_atama,sizeof(deger_atama),fp1)!=NULL)
            {
                if (strstr(deger_atama,"\"etki_degeri\""))
                {
                    sscanf(strstr(deger_atama,"\"etki_degeri\":") + 15,"\"%d\"",&ork->Buz_Devleri.etki_degeri);
                }
                else if(strstr(deger_atama,"\"etki_turu\""))
                {
                    sscanf(deger_atama, " \"etki_turu\": \"%[^\"]\",", ork->Buz_Devleri.etki_turu);
                }

                else if (strstr(deger_atama,"\"aciklama\""))
                {

                    sscanf(deger_atama, " \"aciklama\": \"%[^\"]\",", ork->Buz_Devleri.aciklama);
                }


                if(strstr(deger_atama,"}"))
                {

                    break  ;
                }
            }
        }


}

}

void arastirma_seviye_ozellik_tanimlama(FILE *fp2,struct taraf1 *insan,struct taraf2 *ork){


    char dizi[256];
    char deger_atama[256];

    while (fgets(dizi, sizeof(dizi), fp2) != NULL) {
        if (strstr(dizi, "\"savunma_ustaligi\"")) {

            while (fgets(deger_atama, sizeof(deger_atama), fp2) != NULL) {

                if (strstr(deger_atama, "\"seviye_1\"")) {

                    while (fgets(deger_atama, sizeof(deger_atama), fp2) != NULL) {

                        if (strstr(deger_atama, "\"deger\"")) {

                            sscanf(deger_atama, " \"deger\": \"%d\",", &insan->insan_arastirmalari.savunma_ustaligi.seviye1.deger);
                            ork->ork_arastirmalari.savunma_ustaligi.seviye1.deger = insan->insan_arastirmalari.savunma_ustaligi.seviye1.deger ;
                        }else if (strstr(deger_atama, "\"aciklama\"")) {

                            sscanf(deger_atama, " \"aciklama\": \"%[^\"]\",", insan->insan_arastirmalari.savunma_ustaligi.seviye1.aciklama);
                            strcpy(ork->ork_arastirmalari.savunma_ustaligi.seviye1.aciklama,insan->insan_arastirmalari.savunma_ustaligi.seviye1.aciklama)  ;
                        }
                        if (strstr(deger_atama, "}")) break;
                    }
                }
                else if (strstr(deger_atama, "\"seviye_2\"")) {

                    while (fgets(deger_atama, sizeof(deger_atama), fp2) != NULL) {

                        if (strstr(deger_atama, "\"deger\"")) {

                            sscanf(deger_atama, " \"deger\": \"%d\",", &insan->insan_arastirmalari.savunma_ustaligi.seviye2.deger);
                            ork->ork_arastirmalari.savunma_ustaligi.seviye2.deger = insan->insan_arastirmalari.savunma_ustaligi.seviye2.deger ;

                        }else if (strstr(deger_atama, "\"aciklama\"")) {

                            sscanf(deger_atama, " \"aciklama\": \"%[^\"]\",", insan->insan_arastirmalari.savunma_ustaligi.seviye2.aciklama);
                            strcpy(ork->ork_arastirmalari.savunma_ustaligi.seviye2.aciklama,insan->insan_arastirmalari.savunma_ustaligi.seviye2.aciklama)  ;

                        }
                        if (strstr(deger_atama, "}")) break;
                    }


                }

                if (strstr(deger_atama, "\"seviye_3\"")) {
                    while (fgets(deger_atama, sizeof(deger_atama), fp2) != NULL) {
                        if (strstr(deger_atama, "\"deger\"")) {

                            sscanf(deger_atama, " \"deger\": \"%d\",", &insan->insan_arastirmalari.savunma_ustaligi.seviye3.deger);
                            ork->ork_arastirmalari.savunma_ustaligi.seviye3.deger = insan->insan_arastirmalari.savunma_ustaligi.seviye3.deger ;

                        } else if (strstr(deger_atama, "\"aciklama\"")) {

                            sscanf(deger_atama, " \"aciklama\": \"%[^\"]\",", insan->insan_arastirmalari.savunma_ustaligi.seviye3.aciklama);
                            strcpy(ork->ork_arastirmalari.savunma_ustaligi.seviye3.aciklama,insan->insan_arastirmalari.savunma_ustaligi.seviye3.aciklama)  ;

                        }
                        if (strstr(deger_atama, "}")) break;
                    }
                        if (strstr(deger_atama, "}")) break;

                }

            }


        }


        if (strstr(dizi, "\"saldiri_gelistirmesi\"")) {

            while (fgets(deger_atama, sizeof(deger_atama), fp2) != NULL) {

                if (strstr(deger_atama, "\"seviye_1\"")) {

                    while (fgets(deger_atama, sizeof(deger_atama), fp2) != NULL) {

                        if (strstr(deger_atama, "\"deger\"")) {

                            sscanf(deger_atama, " \"deger\": \"%d\",", &insan->insan_arastirmalari.saldiri_gelistirmesi.seviye1.deger);
                            ork->ork_arastirmalari.saldiri_gelistirmesi.seviye1.deger = insan->insan_arastirmalari.saldiri_gelistirmesi.seviye1.deger;

                        } else if (strstr(deger_atama, "\"aciklama\"")) {

                            sscanf(deger_atama, " \"aciklama\": \"%[^\"]\",", insan->insan_arastirmalari.saldiri_gelistirmesi.seviye1.aciklama);
                            strcpy(ork->ork_arastirmalari.saldiri_gelistirmesi.seviye1.aciklama,insan->insan_arastirmalari.saldiri_gelistirmesi.seviye1.aciklama)  ;

                        }
                        if (strstr(deger_atama, "}")) break;
                    }
                }
                if (strstr(deger_atama, "\"seviye_2\"")) {

                    while (fgets(deger_atama, sizeof(deger_atama), fp2) != NULL) {

                        if (strstr(deger_atama, "\"deger\"")) {

                            sscanf(deger_atama, " \"deger\": \"%d\",", &insan->insan_arastirmalari.saldiri_gelistirmesi.seviye2.deger);
                            ork->ork_arastirmalari.saldiri_gelistirmesi.seviye2.deger = insan->insan_arastirmalari.saldiri_gelistirmesi.seviye2.deger;


                        } else if (strstr(deger_atama, "\"aciklama\"")) {

                            sscanf(deger_atama, " \"aciklama\": \"%[^\"]\",", insan->insan_arastirmalari.saldiri_gelistirmesi.seviye2.aciklama);
                            strcpy(ork->ork_arastirmalari.saldiri_gelistirmesi.seviye2.aciklama,insan->insan_arastirmalari.saldiri_gelistirmesi.seviye2.aciklama)  ;


                        }
                        if (strstr(deger_atama, "}")) break;
                    }
                }
                if (strstr(deger_atama, "\"seviye_3\"")) {

                    while (fgets(deger_atama, sizeof(deger_atama), fp2) != NULL) {

                        if (strstr(deger_atama, "\"deger\"")) {

                            sscanf(deger_atama, " \"deger\": \"%d\",", &insan->insan_arastirmalari.saldiri_gelistirmesi.seviye3.deger);
                            ork->ork_arastirmalari.saldiri_gelistirmesi.seviye3.deger = insan->insan_arastirmalari.saldiri_gelistirmesi.seviye3.deger;


                        } else if (strstr(deger_atama, "\"aciklama\"")) {

                            sscanf(deger_atama, " \"aciklama\": \"%[^\"]\",", insan->insan_arastirmalari.saldiri_gelistirmesi.seviye3.aciklama);
                            strcpy(ork->ork_arastirmalari.saldiri_gelistirmesi.seviye3.aciklama,insan->insan_arastirmalari.saldiri_gelistirmesi.seviye3.aciklama)  ;


                        }
                        if (strstr(deger_atama, "}")) break;
                    }
                                            if (strstr(deger_atama, "}")) break;

                }
            }

        }

        if (strstr(dizi, "\"elit_egitim\"")) {

            while (fgets(deger_atama, sizeof(deger_atama), fp2) != NULL) {

                if (strstr(deger_atama, "\"seviye_1\"")) {

                    while (fgets(deger_atama, sizeof(deger_atama), fp2) != NULL) {

                        if (strstr(deger_atama, "\"deger\"")) {

                            sscanf(deger_atama, " \"deger\": \"%d\",", &insan->insan_arastirmalari.elit_egitim.seviye1.deger);
                            ork->ork_arastirmalari.elit_egitim.seviye1.deger = insan->insan_arastirmalari.elit_egitim.seviye1.deger;


                        } else if (strstr(deger_atama, "\"aciklama\"")) {

                            sscanf(deger_atama, " \"aciklama\": \"%[^\"]\"", insan->insan_arastirmalari.elit_egitim.seviye1.aciklama);
                            strcpy(ork->ork_arastirmalari.elit_egitim.seviye1.aciklama,insan->insan_arastirmalari.elit_egitim.seviye1.aciklama)  ;


                        }
                        if (strstr(deger_atama, "}")) break;
                    }
                }
                if (strstr(deger_atama, "\"seviye_2\"")) {

                    while (fgets(deger_atama, sizeof(deger_atama), fp2) != NULL) {

                        if (strstr(deger_atama, "\"deger\"")) {

                            sscanf(deger_atama, " \"deger\": \"%d\",", &insan->insan_arastirmalari.elit_egitim.seviye2.deger);
                            ork->ork_arastirmalari.elit_egitim.seviye2.deger = insan->insan_arastirmalari.elit_egitim.seviye2.deger;


                        } else if (strstr(deger_atama, "\"aciklama\"")) {

                            sscanf(deger_atama, " \"aciklama\": \"%[^\"]\",", insan->insan_arastirmalari.elit_egitim.seviye2.aciklama);
                            strcpy(ork->ork_arastirmalari.elit_egitim.seviye2.aciklama,insan->insan_arastirmalari.elit_egitim.seviye2.aciklama)  ;


                        }
                        if (strstr(deger_atama, "}")) break;
                    }
                }
                if (strstr(deger_atama, "\"seviye_3\"")) {

                    while (fgets(deger_atama, sizeof(deger_atama), fp2) != NULL) {

                        if (strstr(deger_atama, "\"deger\"")) {

                            sscanf(deger_atama, " \"deger\": \"%d\",", &insan->insan_arastirmalari.elit_egitim.seviye3.deger);
                            ork->ork_arastirmalari.elit_egitim.seviye3.deger = insan->insan_arastirmalari.elit_egitim.seviye3.deger;


                        } else if (strstr(deger_atama, "\"aciklama\"")) {

                            sscanf(deger_atama, " \"aciklama\": \"%[^\"]\",", insan->insan_arastirmalari.elit_egitim.seviye3.aciklama);
                            strcpy(ork->ork_arastirmalari.elit_egitim.seviye3.aciklama,insan->insan_arastirmalari.elit_egitim.seviye3.aciklama)  ;


                        }
                        if (strstr(deger_atama, "}")) break;
                    }
                                            if (strstr(deger_atama, "}")) break;

                }

            }

        }

        if (strstr(dizi, "\"kusatma_ustaligi\"")) {

            while (fgets(deger_atama, sizeof(deger_atama), fp2) != NULL) {

                if (strstr(deger_atama, "\"seviye_1\"")) {

                    while (fgets(deger_atama, sizeof(deger_atama), fp2) != NULL) {

                        if (strstr(deger_atama, "\"deger\"")) {

                            sscanf(deger_atama, " \"deger\": \"%d\",", &insan->insan_arastirmalari.kusatma_ustaligi.seviye1.deger);
                            ork->ork_arastirmalari.kusatma_ustaligi.seviye1.deger = insan->insan_arastirmalari.kusatma_ustaligi.seviye1.deger;


                        } else if (strstr(deger_atama, "\"aciklama\"")) {

                            sscanf(deger_atama, " \"aciklama\": \"%[^\"]\",", insan->insan_arastirmalari.kusatma_ustaligi.seviye1.aciklama);
                            strcpy(ork->ork_arastirmalari.kusatma_ustaligi.seviye1.aciklama,insan->insan_arastirmalari.kusatma_ustaligi.seviye1.aciklama)  ;


                        }
                        if (strstr(deger_atama, "}")) break;
                    }
                }
                if (strstr(deger_atama, "\"seviye_2\"")) {

                    while (fgets(deger_atama, sizeof(deger_atama), fp2) != NULL) {

                        if (strstr(deger_atama, "\"deger\"")) {

                            sscanf(deger_atama, " \"deger\": \"%d\",", &insan->insan_arastirmalari.kusatma_ustaligi.seviye2.deger);
                            ork->ork_arastirmalari.kusatma_ustaligi.seviye2.deger = insan->insan_arastirmalari.kusatma_ustaligi.seviye2.deger;


                        }else if (strstr(deger_atama, "\"aciklama\"")) {

                            sscanf(deger_atama, " \"aciklama\": \"%[^\"]\",", insan->insan_arastirmalari.kusatma_ustaligi.seviye2.aciklama);
                            strcpy(ork->ork_arastirmalari.kusatma_ustaligi.seviye2.aciklama,insan->insan_arastirmalari.kusatma_ustaligi.seviye2.aciklama)  ;


                        }
                        if (strstr(deger_atama, "}")) break;
                    }
                }
                if (strstr(deger_atama, "\"seviye_3\"")) {

                    while (fgets(deger_atama, sizeof(deger_atama), fp2) != NULL) {

                        if (strstr(deger_atama, "\"deger\"")) {

                            sscanf(deger_atama, " \"deger\": \"%d\",", &insan->insan_arastirmalari.kusatma_ustaligi.seviye3.deger);
                            ork->ork_arastirmalari.kusatma_ustaligi.seviye3.deger = insan->insan_arastirmalari.kusatma_ustaligi.seviye3.deger;

                        } else if (strstr(deger_atama, "\"aciklama\"")) {

                            sscanf(deger_atama, " \"aciklama\": \"%[^\"]\",", insan->insan_arastirmalari.kusatma_ustaligi.seviye3.aciklama);
                            strcpy(ork->ork_arastirmalari.kusatma_ustaligi.seviye3.aciklama,insan->insan_arastirmalari.kusatma_ustaligi.seviye3.aciklama)  ;

                        }
                        if (strstr(deger_atama, "}")) break;
                    }
                                            if (strstr(deger_atama, "}")) break;

                }

            }

        }
}
}

void arastirmalari_aktif_et(char *arastirma_seviyesi_section, struct arastirma *arastirmalar) {

    char *ptr = strstr(arastirma_seviyesi_section, "\"savunma_ustaligi\"");
    if (ptr != NULL) {
        int seviye;
        if (sscanf(ptr, "\"savunma_ustaligi\":%d", &seviye) == 1) {
            arastirmalar->savunma_ustaligi.arturu_aktiflik = 1;
            if (seviye == 1) arastirmalar->savunma_ustaligi.seviye1.aktiflik_durumu = 1;
            if (seviye == 2) arastirmalar->savunma_ustaligi.seviye2.aktiflik_durumu = 1;
            if (seviye == 3) arastirmalar->savunma_ustaligi.seviye3.aktiflik_durumu = 1;
        }
    }


    ptr = strstr(arastirma_seviyesi_section, "\"saldiri_gelistirmesi\"");
    if (ptr != NULL) {
        int seviye;
        if (sscanf(ptr, "\"saldiri_gelistirmesi\":%d", &seviye) == 1) {
            arastirmalar->saldiri_gelistirmesi.arturu_aktiflik = 1;
            if (seviye == 1) arastirmalar->saldiri_gelistirmesi.seviye1.aktiflik_durumu = 1;
            if (seviye == 2) arastirmalar->saldiri_gelistirmesi.seviye2.aktiflik_durumu = 1;
            if (seviye == 3) arastirmalar->saldiri_gelistirmesi.seviye3.aktiflik_durumu = 1;
        }
    }


    ptr = strstr(arastirma_seviyesi_section, "\"elit_egitim\"");
    if (ptr != NULL) {
        int seviye;
        if (sscanf(ptr, "\"elit_egitim\":%d", &seviye) == 1) {
            arastirmalar->elit_egitim.arturu_aktiflik = 1;
            if (seviye == 1) arastirmalar->elit_egitim.seviye1.aktiflik_durumu = 1;
            if (seviye == 2) arastirmalar->elit_egitim.seviye2.aktiflik_durumu = 1;
            if (seviye == 3) arastirmalar->elit_egitim.seviye3.aktiflik_durumu = 1;
        }
    }


    ptr = strstr(arastirma_seviyesi_section, "\"kusatma_ustaligi\"");
    if (ptr != NULL) {
        int seviye;
        if (sscanf(ptr, "\"kusatma_ustaligi\":%d", &seviye) == 1) {
            arastirmalar->kusatma_ustaligi.arturu_aktiflik = 1;
            if (seviye == 1) arastirmalar->kusatma_ustaligi.seviye1.aktiflik_durumu = 1;
            if (seviye == 2) arastirmalar->kusatma_ustaligi.seviye2.aktiflik_durumu = 1;
            if (seviye == 3) arastirmalar->kusatma_ustaligi.seviye3.aktiflik_durumu = 1;
        }
    }
}

void isimleri_ayir(const char *girdi, char isimler[][50], int *adet) {
    char *kopya = strdup(girdi);
    char *token = strtok(kopya, "\",[] ");
    *adet = 0;
    while (token != NULL) {
        strcpy(isimler[*adet], token);
        (*adet)++;
        token = strtok(NULL, "\",[] ");
    }
    free(kopya);
}

void kahraman_ve_canavarlari_aktif_et(
    struct taraf1 *insan, struct taraf2 *ork,
    char insan_kahramanlar[5][50], int insan_kahraman_sayisi,
    char insan_canavarlar[5][50], int insan_canavar_sayisi,
    char ork_kahramanlar[4][50], int ork_kahraman_sayisi,
    char ork_canavarlar[6][50], int ork_canavar_sayisi
) {
    int i;

    for (i = 0; i < insan_kahraman_sayisi; i++) {
        if (strcmp(insan_kahramanlar[i], "Alparslan") == 0) {
            insan->Alparslan.aktiflik_durumu = 1;
        }else if (strcmp(insan_kahramanlar[i], "Fatih_Sultan_Mehmet") == 0) {
            insan->Fatih_Sultan_Mehmet.aktiflik_durumu = 1;
        }else if (strcmp(insan_kahramanlar[i], "Yavuz_Sultan_Selim") == 0) {
            insan->Yavuz_Sultan_Selim.aktiflik_durumu = 1;
        }else if (strcmp(insan_kahramanlar[i], "Tugrul_Bey") == 0) {
            insan->Tugrul_Bey.aktiflik_durumu = 1;
        }else if(strcmp(insan_kahramanlar[i],"Mete_Han")){
            insan->Mete_Han.aktiflik_durumu = 1 ;
        }
    }


    for (i = 0; i < insan_canavar_sayisi; i++) {
        if (strcmp(insan_canavarlar[i], "Ejderha") == 0) {
            insan->Ejderha.aktiflik_durumu = 1;
        } else if (strcmp(insan_canavarlar[i], "Agri_Dagi_Devleri") == 0) {
            insan->Agri_Dagi_Devleri.aktiflik_durumu = 1;
        } else if (strcmp(insan_canavarlar[i], "Tepegoz") == 0) {
            insan->Tepegoz.aktiflik_durumu = 1;
        } else if (strcmp(insan_canavarlar[i], "Karakurt") == 0) {
            insan->Karakurt.aktiflik_durumu = 1;
        } else if (strcmp(insan_canavarlar[i], "Samur") == 0) {
            insan->Samur.aktiflik_durumu = 1;
        }
    }


    for (i = 0; i < ork_kahraman_sayisi; i++) {
        if (strcmp(ork_kahramanlar[i], "Goruk_Vahsi") == 0) {
            ork->Goruk_Vahsi.aktiflik_durumu = 1;
        } else if (strcmp(ork_kahramanlar[i], "Thruk_Kemikkiran") == 0) {
            ork->Thruk_Kemikkiran.aktiflik_durumu = 1;
        } else if (strcmp(ork_kahramanlar[i], "Vrog_Kafakiran") == 0) {
            ork->Vrog_Kafakiran.aktiflik_durumu = 1;
        } else if (strcmp(ork_kahramanlar[i], "Ugar_Zalim") == 0) {
            ork->Ugar_Zalim.aktiflik_durumu = 1;
        }
    }


    for (i = 0; i < ork_canavar_sayisi; i++) {
        if (strcmp(ork_canavarlar[i], "Kara_Troll") == 0) {
            ork->Kara_Troll.aktiflik_durumu = 1;
        } else if (strcmp(ork_canavarlar[i], "Golge_Kurtlari") == 0) {
            ork->Golge_Kurtlari.aktiflik_durumu = 1;
        } else if (strcmp(ork_canavarlar[i], "Camur_Devleri") == 0) {
            ork->Camur_Devleri.aktiflik_durumu = 1;
        } else if (strcmp(ork_canavarlar[i], "Ates_Iblisi") == 0) {
            ork->Ates_Iblisi.aktiflik_durumu = 1;
        } else if (strcmp(ork_canavarlar[i], "Buz_Devleri") == 0) {
            ork->Buz_Devleri.aktiflik_durumu = 1;
        }
    }
}

void senaryo_veri_cekme(struct taraf1 *insan,struct taraf2 *ork){

    CURL *curl_handle ;
    CURLcode res ;

    struct Hafiza saklama_yeri ;

    saklama_yeri.memory = malloc(1);
    saklama_yeri.boyut = 0 ;

    curl_global_init(CURL_GLOBAL_ALL);

    curl_handle = curl_easy_init();

    curl_easy_setopt(curl_handle, CURLOPT_URL, "https://yapbenzet.org.tr/8.json");
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&saklama_yeri);
    curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYHOST, 0L);


    res = curl_easy_perform(curl_handle);

    if (res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    } else {
      //  printf("%lu bytes dondu\n", (unsigned long)saklama_yeri.boyut);
       // printf("Veri: %s\n", saklama_yeri.memory);  // Çekilen JSON verisi
    }

    char *json_data = strdup(saklama_yeri.memory);

    char savastaki_ork_kahraman[30];
    char savastaki_ork_canavar[30];
    char savastaki_insan_kahraman[30];
    char savastaki_insan_canavar[30];

    if(strstr(json_data,"\"piyadeler\""))
        sscanf(strstr(json_data,"\"piyadeler\":") + 13,"%d",&insan->piyadeler.sayi);
    if(strstr(json_data,"\"okcular\""))
        sscanf(strstr(json_data,"\"okcular\":") + 11,"%d",&insan->okcular.sayi);
    if(strstr(json_data,"\"suvariler\""))
        sscanf(strstr(json_data,"\"suvariler\":") + 13,"%d",&insan->suvariler.sayi);
    if(strstr(json_data,"\"kusatma_makineleri\""))
        sscanf(strstr(json_data,"\"kusatma_makineleri\":") + 22,"%d",&insan->kusatma_makineleri.sayi);
    if(strstr(json_data,"\"ork_dovusculeri\""))
        sscanf(strstr(json_data,"\"ork_dovusculeri\":") + 19,"%d",&ork->ork_dovusculeri.sayi);
    if(strstr(json_data,"\"varg_binicileri\""))
        sscanf(strstr(json_data,"\"varg_binicileri\":") + 19,"%d",&ork->varg_binicileri.sayi);
    if(strstr(json_data,"\"mizrakcilar\""))
        sscanf(strstr(json_data,"\"mizrakcilar\":") + 15,"%d",&ork->mizrakcilar.sayi);
    if(strstr(json_data,"\"troller\""))
        sscanf(strstr(json_data,"\"troller\":") + 11,"%d",&ork->troller.sayi);


    char *insan_section = strstr(json_data, "\"insan_imparatorlugu\"");
    if (insan_section != NULL) {
    char *kahramanlar_ptr = strstr(insan_section, "\"kahramanlar\"");
    if (kahramanlar_ptr != NULL) {

        if (sscanf(kahramanlar_ptr, "\"kahramanlar\": [%*[ \"]%29[^\"]", savastaki_insan_kahraman) == 1) {
         //   printf("Insan Kahraman: %s\n", savastaki_insan_kahraman);
        }
    }
    char *canavarlar_ptr = strstr(insan_section, "\"canavarlar\"");
    if (canavarlar_ptr != NULL) {

        if (sscanf(canavarlar_ptr, "\"canavarlar\": [%*[ \"]%29[^\"]", savastaki_insan_canavar) == 1) {
           // printf("Insan Canavar: %s\n", savastaki_insan_canavar);
        }
    }

     char *arastirma_ptr = strstr(insan_section, "\"arastirma_seviyesi\"");
        if (arastirma_ptr != NULL) {
            char *start = strchr(arastirma_ptr, '{');
            char *end = strchr(arastirma_ptr, '}');
            if (start != NULL && end != NULL && end > start) {
                int length = end - start + 1;
                char insan_arastirma_seviyesi[512];
                strncpy(insan_arastirma_seviyesi, start, length);
                insan_arastirma_seviyesi[length] = '\0';


                arastirmalari_aktif_et(insan_arastirma_seviyesi, &(insan->insan_arastirmalari));

            }
        }
}

    char *ork_section = strstr(json_data, "\"ork_legi\"");
    if (ork_section != NULL) {
    char *kahramanlar_ptr = strstr(ork_section, "\"kahramanlar\"");
    if (kahramanlar_ptr != NULL) {

        if (sscanf(kahramanlar_ptr, "\"kahramanlar\": [%*[ \"]%29[^\"]", savastaki_ork_kahraman) == 1) {
          //  printf("Ork Kahraman: %s\n", savastaki_ork_kahraman);
        }
    }
    char *canavarlar_ptr = strstr(ork_section, "\"canavarlar\"");
    if (canavarlar_ptr != NULL) {

        if (sscanf(canavarlar_ptr, "\"canavarlar\": [%*[ \"]%29[^\"]", savastaki_ork_canavar) == 1) {
         //   printf("Ork Canavar: %s\n", savastaki_ork_canavar);
        }
    }


    char *arastirma_ptr = strstr(ork_section, "\"arastirma_seviyesi\"");
        if (arastirma_ptr != NULL) {
            char *start = strchr(arastirma_ptr, '{');
            char *end = strchr(arastirma_ptr, '}');
            if (start != NULL && end != NULL && end > start) {
                int length = end - start + 1;
                char ork_arastirma_seviyesi[512];
                strncpy(ork_arastirma_seviyesi, start, length);
                ork_arastirma_seviyesi[length] = '\0';


                arastirmalari_aktif_et(ork_arastirma_seviyesi, &(ork->ork_arastirmalari));

            }
        }
}


    char insan_kahramanlar[5][50];
    int insan_kahraman_sayisi = 5;
    isimleri_ayir(savastaki_insan_kahraman, insan_kahramanlar, &insan_kahraman_sayisi);

    char insan_canavarlar[5][50];
    int insan_canavar_sayisi = 5;
    isimleri_ayir(savastaki_insan_canavar, insan_canavarlar, &insan_canavar_sayisi);

    char ork_kahramanlar[4][50];
    int ork_kahraman_sayisi = 4;
    isimleri_ayir(savastaki_ork_kahraman, ork_kahramanlar, &ork_kahraman_sayisi);

    char ork_canavarlar[6][50];
    int ork_canavar_sayisi = 6;
    isimleri_ayir(savastaki_ork_canavar, ork_canavarlar, &ork_canavar_sayisi);

    kahraman_ve_canavarlari_aktif_et(
        insan,ork,
        insan_kahramanlar, insan_kahraman_sayisi,
        insan_canavarlar, insan_canavar_sayisi,
        ork_kahramanlar, ork_kahraman_sayisi,
        ork_canavarlar, ork_canavar_sayisi
    );


    curl_easy_cleanup(curl_handle);
    free(json_data);
    free(saklama_yeri.memory);

    curl_global_cleanup();

}


void kritik_sansi_degistir(struct taraf1 *insan,struct taraf2 *ork){

    insan->piyadeler.kritik_sans = insan->piyadeler.kritik_sans / 100 ;
    insan->okcular.kritik_sans = insan->okcular.kritik_sans / 100 ;
    insan->suvariler.kritik_sans = insan->suvariler.kritik_sans / 100 ;
    insan->kusatma_makineleri.kritik_sans = insan->kusatma_makineleri.kritik_sans / 100 ;

    ork->varg_binicileri.kritik_sans = ork->varg_binicileri.kritik_sans / 100 ;
    ork->mizrakcilar.kritik_sans = ork->mizrakcilar.kritik_sans / 100 ;
    ork->ork_dovusculeri.kritik_sans = ork->ork_dovusculeri.kritik_sans / 100 ;
    ork->troller.kritik_sans = ork->troller.kritik_sans / 100 ;

}



void yorgunluk_hesapla(struct taraf1 *insan, struct taraf2 *ork, int tur) {
    int yorgunluk_etkisi_sayisi = tur / 5;
    double yorgunluk_orani = 0.10;
    double toplam_yorgunluk_orani = yorgunluk_etkisi_sayisi * yorgunluk_orani;


    if (toplam_yorgunluk_orani > 1.0) {
        toplam_yorgunluk_orani = 1.0;
    }

    double carpani = 1.0 - toplam_yorgunluk_orani;


    insan->piyadeler.guncel_saldiri_gucu = insan->piyadeler.saldiri_gucu * carpani;
    insan->piyadeler.guncel_savunma_gucu = insan->piyadeler.savunma_gucu * carpani;

    insan->okcular.guncel_saldiri_gucu = insan->okcular.saldiri_gucu * carpani;
    insan->okcular.guncel_savunma_gucu = insan->okcular.savunma_gucu * carpani;

    insan->suvariler.guncel_saldiri_gucu = insan->suvariler.saldiri_gucu * carpani;
    insan->suvariler.guncel_savunma_gucu = insan->suvariler.savunma_gucu * carpani;

    insan->kusatma_makineleri.guncel_saldiri_gucu = insan->kusatma_makineleri.saldiri_gucu * carpani;
    insan->kusatma_makineleri.guncel_savunma_gucu = insan->kusatma_makineleri.savunma_gucu * carpani;


    ork->ork_dovusculeri.guncel_saldiri_gucu = ork->ork_dovusculeri.saldiri_gucu * carpani;
    ork->ork_dovusculeri.guncel_savunma_gucu = ork->ork_dovusculeri.savunma_gucu * carpani;

    ork->mizrakcilar.guncel_saldiri_gucu = ork->mizrakcilar.saldiri_gucu * carpani;
    ork->mizrakcilar.guncel_savunma_gucu = ork->mizrakcilar.savunma_gucu * carpani;

    ork->varg_binicileri.guncel_saldiri_gucu = ork->varg_binicileri.saldiri_gucu * carpani;
    ork->varg_binicileri.guncel_savunma_gucu = ork->varg_binicileri.savunma_gucu * carpani;

    ork->troller.guncel_saldiri_gucu = ork->troller.saldiri_gucu * carpani;
    ork->troller.guncel_savunma_gucu = ork->troller.savunma_gucu * carpani;
}

void savunma_gucu_hesapla(struct taraf1 *insan, struct taraf2 *ork) {

    insan->piyadeler.toplam_savunma_gucu = insan->piyadeler.sayi * insan->piyadeler.guncel_savunma_gucu;
    insan->okcular.toplam_savunma_gucu = insan->okcular.sayi * insan->okcular.guncel_savunma_gucu;
    insan->suvariler.toplam_savunma_gucu = insan->suvariler.sayi * insan->suvariler.guncel_savunma_gucu;
    insan->kusatma_makineleri.toplam_savunma_gucu = insan->kusatma_makineleri.sayi * insan->kusatma_makineleri.guncel_savunma_gucu;


    ork->ork_dovusculeri.toplam_savunma_gucu = ork->ork_dovusculeri.sayi * ork->ork_dovusculeri.guncel_savunma_gucu;
    ork->mizrakcilar.toplam_savunma_gucu = ork->mizrakcilar.sayi * ork->mizrakcilar.guncel_savunma_gucu;
    ork->varg_binicileri.toplam_savunma_gucu = ork->varg_binicileri.sayi * ork->varg_binicileri.guncel_savunma_gucu;
    ork->troller.toplam_savunma_gucu = ork->troller.sayi * ork->troller.guncel_savunma_gucu;
}

void saldiri_gucu_hesapla(struct taraf1 *insan, struct taraf2 *ork) {

    insan->piyadeler.toplam_saldiri_gucu = insan->piyadeler.sayi * insan->piyadeler.guncel_saldiri_gucu;
    if (insan->piyadeler.kritik_vurus_oldu_mu) {
        insan->piyadeler.toplam_saldiri_gucu *= 1.5;
    }
    insan->okcular.toplam_saldiri_gucu = insan->okcular.sayi * insan->okcular.guncel_saldiri_gucu;
    if (insan->okcular.kritik_vurus_oldu_mu) {
        insan->okcular.toplam_saldiri_gucu *= 1.5;
    }
    insan->suvariler.toplam_saldiri_gucu = insan->suvariler.sayi * insan->suvariler.guncel_saldiri_gucu;
    if (insan->suvariler.kritik_vurus_oldu_mu) {
        insan->suvariler.toplam_saldiri_gucu *= 1.5;
    }
    insan->kusatma_makineleri.toplam_saldiri_gucu = insan->kusatma_makineleri.sayi * insan->kusatma_makineleri.guncel_saldiri_gucu;


    ork->ork_dovusculeri.toplam_saldiri_gucu = ork->ork_dovusculeri.sayi * ork->ork_dovusculeri.guncel_saldiri_gucu;
    if (ork->ork_dovusculeri.kritik_vurus_oldu_mu) {
        ork->ork_dovusculeri.toplam_saldiri_gucu *= 1.5;
    }
    ork->mizrakcilar.toplam_saldiri_gucu = ork->mizrakcilar.sayi * ork->mizrakcilar.guncel_saldiri_gucu;
    if (ork->mizrakcilar.kritik_vurus_oldu_mu) {
        ork->mizrakcilar.toplam_saldiri_gucu *= 1.5;
    }
    ork->varg_binicileri.toplam_saldiri_gucu = ork->varg_binicileri.sayi * ork->varg_binicileri.guncel_saldiri_gucu;
    if (ork->varg_binicileri.kritik_vurus_oldu_mu) {
        ork->varg_binicileri.toplam_saldiri_gucu *= 1.5;
    }
    ork->troller.toplam_saldiri_gucu = ork->troller.sayi * ork->troller.guncel_saldiri_gucu;
    if (ork->troller.kritik_vurus_oldu_mu) {
        ork->troller.toplam_saldiri_gucu *= 1.5;
    }
}

void kritik_vurus_hesapla(struct Birim *birim) {

    int kritik_vurus_sikligi = (int)(1 / birim->kritik_sans);

    birim->saldiri_sayaci++;

    if (birim->saldiri_sayaci >= kritik_vurus_sikligi) {
        birim->kritik_vurus_oldu_mu = 1;
        birim->saldiri_sayaci = 0;
    } else {
        birim->kritik_vurus_oldu_mu = 0;
    }
}


void saglik_guncelle(struct Birim *birim, double hasar) {

    double birim_saglik = birim->saglik;

    double toplam_mevcut_saglik = birim->sayi * birim_saglik;

    double yeni_toplam_saglik = toplam_mevcut_saglik - hasar;

    if (yeni_toplam_saglik < 0) {
        yeni_toplam_saglik = 0;
    }

    int yeni_birim_sayi = (int)(yeni_toplam_saglik / birim_saglik);

    double kalan_saglik = yeni_toplam_saglik - (yeni_birim_sayi * birim_saglik);


    birim->sayi = yeni_birim_sayi;

    if (birim->sayi <= 0) {
        birim->sayi = 0;
        birim->toplam_saldiri_gucu = 0;
        birim->toplam_savunma_gucu = 0;
    } else {
        birim->toplam_saldiri_gucu = birim->sayi * birim->guncel_saldiri_gucu;
        birim->toplam_savunma_gucu = birim->sayi * birim->guncel_savunma_gucu;
    }
}

void hasar_dagilimi(struct Birim *birimler[], int birim_sayisi, double net_hasar,double birim_hasarlari[4],double birim_oranlari[4]) {
    double toplam_savunma_gucu = 0;
    for (int i = 0; i < birim_sayisi; i++) {
        toplam_savunma_gucu += birimler[i]->toplam_savunma_gucu;
    }

    for (int i = 0; i < birim_sayisi; i++) {
        double oran = birimler[i]->toplam_savunma_gucu / toplam_savunma_gucu;
        birim_oranlari[i] = oran ;
        double birim_hasari = net_hasar * oran;
        birim_hasarlari[i] = birim_hasari ;


        saglik_guncelle(birimler[i], birim_hasari);
    }
}

double net_hasar_hesapla(double toplam_saldiri_gucu, double toplam_savunma_gucu) {
    double net_hasar = toplam_saldiri_gucu * (1 - (toplam_savunma_gucu / toplam_saldiri_gucu));

    if (net_hasar < 0) net_hasar = 0;

    return net_hasar;
}

void kahraman_etkisi_uygula(struct hero *herolar[],struct taraf1 *insan,struct taraf2 *ork,int birim_sayisi){

    for(int i=0; i <birim_sayisi;i++){
        if(herolar[i]->aktiflik_durumu){
        double bonus = 0.00 ;
        if(strstr(herolar[i]->bonus_turu,"savunma")){
        bonus = ((double) herolar[i]->bonus_degeri) / 100.00 ;
        bonus += 1.00 ;

        if(strstr(herolar[i]->aciklama,"piyade")){
                insan->piyadeler.guncel_savunma_gucu *= bonus  ;
        }else if(strstr(herolar[i]->aciklama,"okcu")){
                insan->okcular.guncel_savunma_gucu *= bonus  ;
        }else if(strstr(herolar[i]->aciklama,"suvari")){
                insan->suvariler.guncel_savunma_gucu *= bonus  ;
        }else if(strstr(herolar[i]->aciklama,"kusatma_makine")){
                insan->kusatma_makineleri.guncel_savunma_gucu *=bonus ;
        }else if(strstr(herolar[i]->aciklama,"ork_dovuscu")){
                ork->ork_dovusculeri.guncel_savunma_gucu *= bonus ;
        }else if(strstr(herolar[i]->aciklama,"troller")){
                ork->troller.guncel_savunma_gucu *= bonus ;
        }else if(strstr(herolar[i]->aciklama,"varg_binici")){
                ork->varg_binicileri.guncel_savunma_gucu *= bonus ;
        }else if(strstr(herolar[i]->aciklama,"tum_birimler")){
                ork->mizrakcilar.guncel_savunma_gucu *= bonus ;
                ork->ork_dovusculeri.guncel_savunma_gucu *= bonus ;
                ork->troller.guncel_savunma_gucu *= bonus ;
                ork->varg_binicileri.guncel_savunma_gucu *= bonus ;
                }

    }if(strstr(herolar[i]->bonus_turu,"saldiri")){
        bonus = ((double) herolar[i]->bonus_degeri) / 100.00 ;
        bonus += 1.00 ;
        if(strstr(herolar[i]->aciklama,"piyade")){
                insan->piyadeler.guncel_saldiri_gucu *= bonus  ;
        }else if(strstr(herolar[i]->aciklama,"okcu")){
                insan->okcular.guncel_saldiri_gucu *= bonus  ;
        }else if(strstr(herolar[i]->aciklama,"suvari")){
                insan->suvariler.guncel_saldiri_gucu *= bonus  ;
        }else if(strstr(herolar[i]->aciklama,"kusatma_makine")){
                insan->kusatma_makineleri.guncel_saldiri_gucu *=bonus ;
        }else if(strstr(herolar[i]->aciklama,"ork_dovuscu")){
                ork->ork_dovusculeri.guncel_saldiri_gucu *= bonus ;
        }else if(strstr(herolar[i]->aciklama,"troller")){
                ork->troller.guncel_saldiri_gucu *= bonus ;
        }else if(strstr(herolar[i]->aciklama,"varg_binici")){
                ork->varg_binicileri.guncel_saldiri_gucu *= bonus ;
        }else if(strstr(herolar[i]->aciklama,"tum_birimler")){
                ork->mizrakcilar.guncel_saldiri_gucu *= bonus ;
                ork->ork_dovusculeri.guncel_saldiri_gucu *= bonus ;
                ork->troller.guncel_saldiri_gucu *= bonus ;
                ork->varg_binicileri.guncel_saldiri_gucu *= bonus ;
                }

    }if(strstr(herolar[i]->bonus_turu,"kritik_sans")){
        bonus = ((double) herolar[i]->bonus_degeri) / 100.00 ;

        if(strstr(herolar[i]->aciklama,"piyade")){
                insan->piyadeler.kritik_sans += bonus  ;
        }else if(strstr(herolar[i]->aciklama,"okcu")){
                insan->okcular.kritik_sans += bonus  ;
        }else if(strstr(herolar[i]->aciklama,"suvari")){
                insan->suvariler.kritik_sans += bonus  ;
        }else if(strstr(herolar[i]->aciklama,"kusatma_makine")){
                insan->kusatma_makineleri.kritik_sans += bonus ;
        }else if(strstr(herolar[i]->aciklama,"ork_dovuscu")){
                ork->ork_dovusculeri.kritik_sans += bonus ;
        }else if(strstr(herolar[i]->aciklama,"troller")){
                ork->troller.kritik_sans += bonus ;
        }else if(strstr(herolar[i]->aciklama,"varg_binici")){
                ork->varg_binicileri.kritik_sans += bonus ;
        }else if(strstr(herolar[i]->aciklama,"tum_birimler")){
                ork->mizrakcilar.kritik_sans += bonus ;
                ork->ork_dovusculeri.kritik_sans += bonus ;
                ork->troller.kritik_sans += bonus ;
                ork->varg_binicileri.kritik_sans += bonus ;
                }

    }
    }


}

}

void canavar_etkisi_uygula(struct creature *creatureslar[],struct taraf1 *insan,struct taraf2 *ork,int birim_sayisi){

 for(int i=0; i <birim_sayisi;i++){
        if(creatureslar[i]->aktiflik_durumu){
        double bonus = 0.00 ;
        if(strstr(creatureslar[i]->etki_turu,"savunma")){
        bonus = ((double) creatureslar[i]->etki_degeri) / 100.00 ;
        bonus += 1.00 ;
        if(strstr(creatureslar[i]->aciklama,"piyade")){
                insan->piyadeler.guncel_savunma_gucu *= bonus  ;
        }else if(strstr(creatureslar[i]->aciklama,"okcu")){
                insan->okcular.guncel_savunma_gucu *= bonus  ;
        }else if(strstr(creatureslar[i]->aciklama,"suvari")){
                insan->suvariler.guncel_savunma_gucu *= bonus  ;
        }else if(strstr(creatureslar[i]->aciklama,"kusatma_makine")){
                insan->kusatma_makineleri.guncel_savunma_gucu *=bonus ;
        }else if(strstr(creatureslar[i]->aciklama,"ork_dovuscu")){
                ork->ork_dovusculeri.guncel_savunma_gucu *= bonus ;
        }else if(strstr(creatureslar[i]->aciklama,"troller")){
                ork->troller.guncel_savunma_gucu *= bonus ;
        }else if(strstr(creatureslar[i]->aciklama,"varg_binici")){
                ork->varg_binicileri.guncel_savunma_gucu *= bonus ;
        }else if(strstr(creatureslar[i]->aciklama,"tum_birimler")){
                ork->mizrakcilar.guncel_savunma_gucu *= bonus ;
                ork->ork_dovusculeri.guncel_savunma_gucu *= bonus ;
                ork->troller.guncel_savunma_gucu *= bonus ;
                ork->varg_binicileri.guncel_savunma_gucu *= bonus ;
                }

    }if(strstr(creatureslar[i]->etki_turu,"saldiri")){
        bonus = ((double) creatureslar[i]->etki_degeri) / 100.00 ;
        bonus += 1.00 ;
        if(strstr(creatureslar[i]->aciklama,"piyade")){
                insan->piyadeler.guncel_saldiri_gucu *= bonus  ;
        }else if(strstr(creatureslar[i]->aciklama,"okcu")){
                insan->okcular.guncel_saldiri_gucu *= bonus  ;
        }else if(strstr(creatureslar[i]->aciklama,"suvari")){
                insan->suvariler.guncel_saldiri_gucu *= bonus  ;
        }else if(strstr(creatureslar[i]->aciklama,"kusatma_makine")){
                insan->kusatma_makineleri.guncel_saldiri_gucu *=bonus ;
        }else if(strstr(creatureslar[i]->aciklama,"ork_dovuscu")){
                ork->ork_dovusculeri.guncel_saldiri_gucu *= bonus ;
        }else if(strstr(creatureslar[i]->aciklama,"troller")){
                ork->troller.guncel_saldiri_gucu *= bonus ;
        }else if(strstr(creatureslar[i]->aciklama,"varg_binici")){
                ork->varg_binicileri.guncel_saldiri_gucu *= bonus ;
        }else if(strstr(creatureslar[i]->aciklama,"tum_birimler")){
                ork->mizrakcilar.guncel_saldiri_gucu *= bonus ;
                ork->ork_dovusculeri.guncel_saldiri_gucu *= bonus ;
                ork->troller.guncel_saldiri_gucu *= bonus ;
                ork->varg_binicileri.guncel_saldiri_gucu *= bonus ;
                }

    }if(strstr(creatureslar[i]->etki_turu,"kritik_sans")){
        bonus = ((double) creatureslar[i]->etki_degeri) / 100.00 ;

        if(strstr(creatureslar[i]->aciklama,"piyade")){
                insan->piyadeler.kritik_sans += bonus  ;
        }else if(strstr(creatureslar[i]->aciklama,"okcu")){
                insan->okcular.kritik_sans += bonus  ;
        }else if(strstr(creatureslar[i]->aciklama,"suvari")){
                insan->suvariler.kritik_sans += bonus  ;
        }else if(strstr(creatureslar[i]->aciklama,"kusatma_makine")){
                insan->kusatma_makineleri.kritik_sans += bonus ;
        }else if(strstr(creatureslar[i]->aciklama,"ork_dovuscu")){
                ork->ork_dovusculeri.kritik_sans += bonus ;
        }else if(strstr(creatureslar[i]->aciklama,"troller")){
                ork->troller.kritik_sans += bonus ;
        }else if(strstr(creatureslar[i]->aciklama,"varg_binici")){
                ork->varg_binicileri.kritik_sans += bonus ;
        }else if(strstr(creatureslar[i]->aciklama,"tum_birimler")){
                ork->mizrakcilar.kritik_sans += bonus ;
                ork->ork_dovusculeri.kritik_sans += bonus ;
                ork->troller.kritik_sans += bonus ;
                ork->varg_binicileri.kritik_sans += bonus ;
                }

    }
    }


}

}

void arastirma_etkisi_uygula(struct taraf1 *insan_tarafi,struct taraf2 *ork_tarafi){
    double bonus = 0.00 ;

    if(insan_tarafi->insan_arastirmalari.savunma_ustaligi.arturu_aktiflik){
            bonus += 1.00 ;
            if(insan_tarafi->insan_arastirmalari.savunma_ustaligi.seviye1.aktiflik_durumu){
              bonus = bonus + (((double)insan_tarafi->insan_arastirmalari.savunma_ustaligi.seviye1.deger) / 100.00) ;
              insan_tarafi->piyadeler.guncel_savunma_gucu *= bonus ;
              insan_tarafi->okcular.guncel_savunma_gucu *= bonus ;
              insan_tarafi->suvariler.guncel_savunma_gucu *= bonus ;
              insan_tarafi->kusatma_makineleri.guncel_savunma_gucu *= bonus ;
            }else if(insan_tarafi->insan_arastirmalari.savunma_ustaligi.seviye2.aktiflik_durumu){
                bonus = bonus + (((double)insan_tarafi->insan_arastirmalari.savunma_ustaligi.seviye2.deger) / 100.00) ;
                insan_tarafi->piyadeler.guncel_savunma_gucu *= bonus ;
                insan_tarafi->okcular.guncel_savunma_gucu *= bonus ;
                insan_tarafi->suvariler.guncel_savunma_gucu *= bonus ;
                insan_tarafi->kusatma_makineleri.guncel_savunma_gucu *= bonus ;
            }else if(insan_tarafi->insan_arastirmalari.savunma_ustaligi.seviye3.aktiflik_durumu){
                bonus = bonus + (((double)insan_tarafi->insan_arastirmalari.savunma_ustaligi.seviye3.deger) / 100.00) ;
                insan_tarafi->piyadeler.guncel_savunma_gucu *= bonus ;
                insan_tarafi->okcular.guncel_savunma_gucu *= bonus ;
                insan_tarafi->suvariler.guncel_savunma_gucu *= bonus ;
                insan_tarafi->kusatma_makineleri.guncel_savunma_gucu *= bonus ;
            }
    }else if(insan_tarafi->insan_arastirmalari.saldiri_gelistirmesi.arturu_aktiflik){
            bonus += 1.00 ;
            if(insan_tarafi->insan_arastirmalari.saldiri_gelistirmesi.seviye1.aktiflik_durumu){
              bonus = bonus + (((double)insan_tarafi->insan_arastirmalari.saldiri_gelistirmesi.seviye1.deger) / 100.00) ;
              insan_tarafi->piyadeler.guncel_saldiri_gucu *= bonus ;
              insan_tarafi->okcular.guncel_saldiri_gucu *= bonus ;
              insan_tarafi->suvariler.guncel_saldiri_gucu *= bonus ;
              insan_tarafi->kusatma_makineleri.guncel_saldiri_gucu *= bonus ;
            }else if(insan_tarafi->insan_arastirmalari.saldiri_gelistirmesi.seviye2.aktiflik_durumu){
              bonus = bonus + (((double)insan_tarafi->insan_arastirmalari.saldiri_gelistirmesi.seviye2.deger) / 100.00) ;
              insan_tarafi->piyadeler.guncel_saldiri_gucu *= bonus ;
              insan_tarafi->okcular.guncel_saldiri_gucu *= bonus ;
              insan_tarafi->suvariler.guncel_saldiri_gucu *= bonus ;
              insan_tarafi->kusatma_makineleri.guncel_saldiri_gucu *= bonus ;
            }else if(insan_tarafi->insan_arastirmalari.saldiri_gelistirmesi.seviye3.aktiflik_durumu){
              bonus = bonus + (((double)insan_tarafi->insan_arastirmalari.saldiri_gelistirmesi.seviye3.deger) / 100.00) ;
              insan_tarafi->piyadeler.guncel_saldiri_gucu *= bonus ;
              insan_tarafi->okcular.guncel_saldiri_gucu *= bonus ;
              insan_tarafi->suvariler.guncel_saldiri_gucu *= bonus ;
              insan_tarafi->kusatma_makineleri.guncel_saldiri_gucu *= bonus ;
            }
    }else if(insan_tarafi->insan_arastirmalari.elit_egitim.arturu_aktiflik){
        bonus = 0.00 ;
            if(insan_tarafi->insan_arastirmalari.elit_egitim.seviye1.aktiflik_durumu){
              bonus = bonus + (((double)insan_tarafi->insan_arastirmalari.elit_egitim.seviye1.deger) / 100.00) ;
              insan_tarafi->piyadeler.kritik_sans += bonus ;
              insan_tarafi->okcular.kritik_sans += bonus ;
              insan_tarafi->suvariler.kritik_sans += bonus ;
              insan_tarafi->kusatma_makineleri.kritik_sans += bonus ;
            }else if(insan_tarafi->insan_arastirmalari.elit_egitim.seviye2.aktiflik_durumu){
              bonus = bonus + (((double)insan_tarafi->insan_arastirmalari.elit_egitim.seviye2.deger) / 100.00) ;
              insan_tarafi->piyadeler.kritik_sans += bonus ;
              insan_tarafi->okcular.kritik_sans += bonus ;
              insan_tarafi->suvariler.kritik_sans += bonus ;
              insan_tarafi->kusatma_makineleri.kritik_sans += bonus ;
            }else if(insan_tarafi->insan_arastirmalari.elit_egitim.seviye3.aktiflik_durumu){
              bonus = bonus + (((double)insan_tarafi->insan_arastirmalari.elit_egitim.seviye3.deger) / 100.00) ;
              insan_tarafi->piyadeler.kritik_sans += bonus ;
              insan_tarafi->okcular.kritik_sans += bonus ;
              insan_tarafi->suvariler.kritik_sans += bonus ;
              insan_tarafi->kusatma_makineleri.kritik_sans += bonus ;
            }

    }else if(insan_tarafi->insan_arastirmalari.kusatma_ustaligi.arturu_aktiflik){
        bonus = 1.00 ;
            if(insan_tarafi->insan_arastirmalari.kusatma_ustaligi.seviye1.aktiflik_durumu){
              bonus = bonus + (((double)insan_tarafi->insan_arastirmalari.kusatma_ustaligi.seviye1.deger) / 100.00) ;
              insan_tarafi->kusatma_makineleri.guncel_saldiri_gucu *= bonus ;
            }else if(insan_tarafi->insan_arastirmalari.kusatma_ustaligi.seviye2.aktiflik_durumu){
              bonus = bonus + (((double)insan_tarafi->insan_arastirmalari.kusatma_ustaligi.seviye2.deger) / 100.00) ;
              insan_tarafi->kusatma_makineleri.guncel_saldiri_gucu *= bonus ;
            }else if(insan_tarafi->insan_arastirmalari.kusatma_ustaligi.seviye3.aktiflik_durumu){
              bonus = bonus + (((double)insan_tarafi->insan_arastirmalari.kusatma_ustaligi.seviye3.deger) / 100.00) ;
              insan_tarafi->kusatma_makineleri.guncel_saldiri_gucu *= bonus ;
            }
    }

    double ork_bonus = 0.00 ;
   if(ork_tarafi->ork_arastirmalari.savunma_ustaligi.arturu_aktiflik){
            ork_bonus = 1.00 ;
            if(ork_tarafi->ork_arastirmalari.savunma_ustaligi.seviye1.aktiflik_durumu){
              ork_bonus = ork_bonus + (((double)ork_tarafi->ork_arastirmalari.savunma_ustaligi.seviye1.deger) / 100.00) ;
              ork_tarafi->ork_dovusculeri.guncel_savunma_gucu *= ork_bonus ;
              ork_tarafi->mizrakcilar.guncel_savunma_gucu *= ork_bonus ;
              ork_tarafi->varg_binicileri.guncel_savunma_gucu *= ork_bonus ;
              ork_tarafi->troller.guncel_savunma_gucu *= ork_bonus ;
            }else if(ork_tarafi->ork_arastirmalari.savunma_ustaligi.seviye2.aktiflik_durumu){
                ork_bonus = ork_bonus + (((double)ork_tarafi->ork_arastirmalari.savunma_ustaligi.seviye2.deger) / 100.00) ;
                ork_tarafi->ork_dovusculeri.guncel_savunma_gucu *= ork_bonus ;
                ork_tarafi->mizrakcilar.guncel_savunma_gucu *= ork_bonus ;
                ork_tarafi->varg_binicileri.guncel_savunma_gucu *= ork_bonus ;
                ork_tarafi->troller.guncel_savunma_gucu *= ork_bonus ;
            }else if(ork_tarafi->ork_arastirmalari.savunma_ustaligi.seviye3.aktiflik_durumu){
                ork_bonus = ork_bonus + (((double)ork_tarafi->ork_arastirmalari.savunma_ustaligi.seviye3.deger) / 100.00) ;
                ork_tarafi->ork_dovusculeri.guncel_savunma_gucu *= ork_bonus ;
                ork_tarafi->mizrakcilar.guncel_savunma_gucu *= ork_bonus ;
                ork_tarafi->varg_binicileri.guncel_savunma_gucu *= ork_bonus ;
                ork_tarafi->troller.guncel_savunma_gucu *= ork_bonus ;
            }

    }else if(ork_tarafi->ork_arastirmalari.saldiri_gelistirmesi.arturu_aktiflik){
            ork_bonus = 1.00 ;
            if(ork_tarafi->ork_arastirmalari.saldiri_gelistirmesi.seviye1.aktiflik_durumu){
              ork_bonus = ork_bonus + (((double)ork_tarafi->ork_arastirmalari.saldiri_gelistirmesi.seviye1.deger) / 100.00) ;
                ork_tarafi->ork_dovusculeri.guncel_saldiri_gucu *= ork_bonus ;
                ork_tarafi->mizrakcilar.guncel_saldiri_gucu *= ork_bonus ;
                ork_tarafi->varg_binicileri.guncel_saldiri_gucu *= ork_bonus ;
                ork_tarafi->troller.guncel_saldiri_gucu *= ork_bonus ;
            }else if(insan_tarafi->insan_arastirmalari.saldiri_gelistirmesi.seviye2.aktiflik_durumu){
              ork_bonus = ork_bonus + (((double)insan_tarafi->insan_arastirmalari.saldiri_gelistirmesi.seviye2.deger) / 100.00) ;
                ork_tarafi->ork_dovusculeri.guncel_saldiri_gucu *= ork_bonus ;
                ork_tarafi->mizrakcilar.guncel_saldiri_gucu *= ork_bonus ;
                ork_tarafi->varg_binicileri.guncel_saldiri_gucu *= ork_bonus ;
                ork_tarafi->troller.guncel_saldiri_gucu *= ork_bonus ;
            }else if(insan_tarafi->insan_arastirmalari.saldiri_gelistirmesi.seviye3.aktiflik_durumu){
              ork_bonus = ork_bonus + (((double)insan_tarafi->insan_arastirmalari.saldiri_gelistirmesi.seviye3.deger) / 100.00) ;
                ork_tarafi->ork_dovusculeri.guncel_saldiri_gucu *= ork_bonus ;
                ork_tarafi->mizrakcilar.guncel_saldiri_gucu *= ork_bonus ;
                ork_tarafi->varg_binicileri.guncel_saldiri_gucu *= ork_bonus ;
                ork_tarafi->troller.guncel_saldiri_gucu *= ork_bonus ;
            }
    }else if(ork_tarafi->ork_arastirmalari.elit_egitim.arturu_aktiflik){
        ork_bonus = 0.00 ;
            if(ork_tarafi->ork_arastirmalari.elit_egitim.seviye1.aktiflik_durumu){
              ork_bonus = ork_bonus + (((double)ork_tarafi->ork_arastirmalari.elit_egitim.seviye1.deger) / 100.00) ;
                ork_tarafi->ork_dovusculeri.kritik_sans += ork_bonus ;
                ork_tarafi->mizrakcilar.kritik_sans += ork_bonus ;
                ork_tarafi->varg_binicileri.kritik_sans += ork_bonus ;
                ork_tarafi->troller.kritik_sans += ork_bonus ;
            }else if(ork_tarafi->ork_arastirmalari.elit_egitim.seviye2.aktiflik_durumu){
              ork_bonus = ork_bonus + (((double)ork_tarafi->ork_arastirmalari.elit_egitim.seviye2.deger) / 100.00) ;
                ork_tarafi->ork_dovusculeri.kritik_sans += ork_bonus ;
                ork_tarafi->mizrakcilar.kritik_sans += ork_bonus ;
                ork_tarafi->varg_binicileri.kritik_sans += ork_bonus ;
                ork_tarafi->troller.kritik_sans += ork_bonus ;
            }else if(ork_tarafi->ork_arastirmalari.elit_egitim.seviye3.aktiflik_durumu){
              ork_bonus = ork_bonus + (((double)ork_tarafi->ork_arastirmalari.elit_egitim.seviye3.deger) / 100.00) ;
                ork_tarafi->ork_dovusculeri.kritik_sans += ork_bonus ;
                ork_tarafi->mizrakcilar.kritik_sans += ork_bonus ;
                ork_tarafi->varg_binicileri.kritik_sans += ork_bonus ;
                ork_tarafi->troller.kritik_sans += ork_bonus ;
            }

    }
}

void yazdir(FILE *dosya,struct taraf1 *insan,struct taraf2 *ork){

    fprintf(dosya,"==========İnsan İmparatorluğu==========\n\n 1. Piyadeler: %d birim(Saldırı Gücü: %d,Savunma Gücü: %d,Sağlık: %d,Kritik Vuruş Şansı: %%%d)\n",insan->piyadeler.sayi,insan->piyadeler.saldiri_gucu,insan->piyadeler.savunma_gucu,insan->piyadeler.saglik,(int)insan->piyadeler.kritik_sans);
    fprintf(dosya," 2. Okçular: %d birim(Saldırı Gücü: %d,Savunma Gücü: %d,Sağlık: %d,Kritik Vuruş Şansı: %%%d)\n",insan->okcular.sayi,insan->okcular.saldiri_gucu,insan->okcular.savunma_gucu,insan->okcular.saglik,(int)insan->okcular.kritik_sans);
    fprintf(dosya," 3. Süvariler: %d birim(Saldırı Gücü: %d,Savunma Gücü: %d,Sağlık: %d,Kritik Vuruş Şansı: %%%d)\n",insan->suvariler.sayi,insan->suvariler.saldiri_gucu,insan->suvariler.savunma_gucu,insan->suvariler.saglik,(int)insan->suvariler.kritik_sans);
    fprintf(dosya," 4. Kuşatma Makineleri: %d birim(Saldırı Gücü: %d,Savunma Gücü: %d,Sağlık: %d,Kritik Vuruş Şansı: %%%d)\n",insan->kusatma_makineleri.sayi,insan->kusatma_makineleri.saldiri_gucu,insan->kusatma_makineleri.savunma_gucu,insan->kusatma_makineleri.saglik,(int)insan->kusatma_makineleri.kritik_sans);
    fprintf(dosya,"Kahraman:\n 1.");
    if(insan->Alparslan.aktiflik_durumu){
     fprintf(dosya,"Alparslan(%s)\n",insan->Alparslan.aciklama);
    }else if(insan->Fatih_Sultan_Mehmet.aktiflik_durumu){
        fprintf(dosya,"Fatih Sultan Mehmet(%s)\n",insan->Fatih_Sultan_Mehmet.aciklama);
    }else if(insan->Tugrul_Bey.aktiflik_durumu){
        fprintf(dosya,"Tuğrul Bey(%s)\n",insan->Tugrul_Bey.aciklama);
    }else if(insan->Yavuz_Sultan_Selim.aktiflik_durumu){
        fprintf(dosya,"Yavuz Sultan Selim(%s)\n",insan->Yavuz_Sultan_Selim.aciklama);
    }else if(insan->Mete_Han.aktiflik_durumu){
        fprintf(dosya,"Mete Han(%s)\n",insan->Mete_Han.aciklama);
    }
    fprintf(dosya,"Canavar:\n 1.");
    if(insan->Ejderha.aktiflik_durumu){
     fprintf(dosya,"Ejderha(%s)\n",insan->Ejderha.aciklama);
    }else if(insan->Karakurt.aktiflik_durumu){
        fprintf(dosya,"Kara Kurt(%s)\n",insan->Karakurt.aciklama);
    }else if(insan->Agri_Dagi_Devleri.aktiflik_durumu){
        fprintf(dosya,"Ağrı Dağı Devleri(%s)\n",insan->Agri_Dagi_Devleri.aciklama);
    }else if(insan->Samur.aktiflik_durumu){
        fprintf(dosya,"Samur(%s)\n",insan->Samur.aciklama);
    }else if(insan->Tepegoz.aktiflik_durumu){
        fprintf(dosya,"Tepegöz(%s)\n",insan->Tepegoz.aciklama);
    }
    fprintf(dosya,"Araştırmalar:\n");
    if(insan->insan_arastirmalari.saldiri_gelistirmesi.arturu_aktiflik){
            if(insan->insan_arastirmalari.saldiri_gelistirmesi.seviye1.aktiflik_durumu){
                    fprintf(dosya,"Saldırı Geliştirmesi(Seviye 1): %s\n\n",insan->insan_arastirmalari.saldiri_gelistirmesi.seviye1.aciklama);
            }else if(insan->insan_arastirmalari.saldiri_gelistirmesi.seviye2.aktiflik_durumu){
                    fprintf(dosya,"Saldırı Geliştirmesi(Seviye 2): %s\n\n",insan->insan_arastirmalari.saldiri_gelistirmesi.seviye2.aciklama);
            }else if(insan->insan_arastirmalari.saldiri_gelistirmesi.seviye3.aktiflik_durumu){
                    fprintf(dosya,"Saldırı Geliştirmesi(Seviye 3): %s\n\n",insan->insan_arastirmalari.saldiri_gelistirmesi.seviye3.aciklama);
            }
    }else if(insan->insan_arastirmalari.savunma_ustaligi.arturu_aktiflik){
            if(insan->insan_arastirmalari.savunma_ustaligi.seviye1.aktiflik_durumu){
                    fprintf(dosya,"Savunma Ustalığı (Seviye 1): %s\n\n",insan->insan_arastirmalari.savunma_ustaligi.seviye1.aciklama);
            }else if(insan->insan_arastirmalari.savunma_ustaligi.seviye2.aktiflik_durumu){
                    fprintf(dosya,"Savunma Ustalığı (Seviye 2): %s\n\n",insan->insan_arastirmalari.savunma_ustaligi.seviye2.aciklama);
            }else if(insan->insan_arastirmalari.savunma_ustaligi.seviye3.aktiflik_durumu){
                    fprintf(dosya,"Savunma Ustalığı (Seviye 3): %s\n\n",insan->insan_arastirmalari.savunma_ustaligi.seviye3.aciklama);
            }
    }else if(insan->insan_arastirmalari.elit_egitim.arturu_aktiflik){
            if(insan->insan_arastirmalari.elit_egitim.seviye1.aktiflik_durumu){
                    fprintf(dosya,"Elit Eğitim (Seviye 1): %s\n\n",insan->insan_arastirmalari.elit_egitim.seviye1.aciklama);
            }else if(insan->insan_arastirmalari.elit_egitim.seviye2.aktiflik_durumu){
                    fprintf(dosya,"Elit Eğitim (Seviye 2): %s\n\n",insan->insan_arastirmalari.elit_egitim.seviye2.aciklama);
            }else if(insan->insan_arastirmalari.elit_egitim.seviye3.aktiflik_durumu){
                    fprintf(dosya,"Elit Eğitim (Seviye 3): %s\n\n",insan->insan_arastirmalari.elit_egitim.seviye3.aciklama);
            }
    }else if(insan->insan_arastirmalari.kusatma_ustaligi.arturu_aktiflik){
            if(insan->insan_arastirmalari.kusatma_ustaligi.seviye1.aktiflik_durumu){
                    fprintf(dosya,"Kuşatma Ustalığı (Seviye 1): %s\n\n",insan->insan_arastirmalari.kusatma_ustaligi.seviye1.aciklama);
            }else if(insan->insan_arastirmalari.kusatma_ustaligi.seviye2.aktiflik_durumu){
                    fprintf(dosya,"Kuşatma Ustalığı (Seviye 2): %s\n\n",insan->insan_arastirmalari.kusatma_ustaligi.seviye2.aciklama);
            }else if(insan->insan_arastirmalari.kusatma_ustaligi.seviye3.aktiflik_durumu){
                    fprintf(dosya,"Kuşatma Ustalığı (Seviye 3): %s\n\n",insan->insan_arastirmalari.kusatma_ustaligi.seviye3.aciklama);
            }
        }
    fprintf(dosya,"==========Ork Lejyonu==========\n\n 1. Ork Dövüşçüleri: %d birim(Saldırı Gücü: %d,Savunma Gücü: %d,Sağlık: %d,Kritik Vuruş Şansı: %%%d)\n",ork->ork_dovusculeri.sayi,ork->ork_dovusculeri.saldiri_gucu,ork->ork_dovusculeri.savunma_gucu,ork->ork_dovusculeri.saglik,(int)ork->ork_dovusculeri.kritik_sans);
    fprintf(dosya," 2. Mızrakçılar: %d birim(Saldırı Gücü: %d,Savunma Gücü: %d,Sağlık: %d,Kritik Vuruş Şansı: %%%d)\n",ork->mizrakcilar.sayi,ork->mizrakcilar.saldiri_gucu,ork->mizrakcilar.savunma_gucu,ork->mizrakcilar.saglik,(int)ork->mizrakcilar.kritik_sans);
    fprintf(dosya," 3. Varg Binicileri: %d birim(Saldırı Gücü: %d,Savunma Gücü: %d,Sağlık: %d,Kritik Vuruş Şansı: %%%d)\n",ork->varg_binicileri.sayi,ork->varg_binicileri.saldiri_gucu,ork->varg_binicileri.savunma_gucu,ork->varg_binicileri.saglik,(int)ork->varg_binicileri.kritik_sans);
    fprintf(dosya," 4. Troller: %d birim(Saldırı Gücü: %d,Savunma Gücü: %d,Sağlık: %d,Kritik Vuruş Şansı: %%%d)\n",ork->troller.sayi,ork->troller.saldiri_gucu,ork->troller.savunma_gucu,ork->troller.saglik,(int)ork->troller.kritik_sans);
    fprintf(dosya,"Kahraman:\n 1.");
    if(ork->Goruk_Vahsi.aktiflik_durumu){
     fprintf(dosya,"Goruk Vahşi(%s)\n",ork->Goruk_Vahsi.aciklama);
    }else if(ork->Thruk_Kemikkiran.aktiflik_durumu){
        fprintf(dosya,"Thruk Kemikkıran(%s)\n",ork->Thruk_Kemikkiran.aciklama);
    }else if(ork->Ugar_Zalim.aktiflik_durumu){
        fprintf(dosya,"Ugar Zalim(%s)\n",ork->Ugar_Zalim.aciklama);
    }else if(ork->Vrog_Kafakiran.aktiflik_durumu){
        fprintf(dosya,"Vrog Kafakıran(%s)\n",ork->Vrog_Kafakiran.aciklama);
    }
    fprintf(dosya,"Canavar:\n 1.");
    if(ork->Ates_Iblisi.aktiflik_durumu){
     fprintf(dosya,"Ateş İblisi(%s)\n",ork->Ates_Iblisi.aciklama);
    }else if(ork->Buz_Devleri.aktiflik_durumu){
        fprintf(dosya,"Buz Devleri(%s)\n",ork->Buz_Devleri.aciklama);
    }else if(ork->Camur_Devleri.aktiflik_durumu){
        fprintf(dosya,"Çamur Devleri Devleri(%s)\n",ork->Camur_Devleri.aciklama);
    }else if(ork->Golge_Kurtlari.aktiflik_durumu){
        fprintf(dosya,"Gölge Kurtlari(%s)\n",ork->Golge_Kurtlari.aciklama);
    }else if(ork->Kara_Troll.aktiflik_durumu){
        fprintf(dosya,"Kara_Troll(%s)\n",ork->Kara_Troll.aciklama);
    }else if(ork->Makrog_Savas_Beyi.aktiflik_durumu){
        fprintf(dosya,"Makrog Savaş Beyi(%s)\n",ork->Makrog_Savas_Beyi.aciklama);
    }

    fprintf(dosya,"Araştırmalar:\n");
    if(ork->ork_arastirmalari.saldiri_gelistirmesi.arturu_aktiflik){
            if(ork->ork_arastirmalari.saldiri_gelistirmesi.seviye1.aktiflik_durumu){
                    fprintf(dosya,"Saldırı Geliştirmesi(Seviye 1): %s\n\n",ork->ork_arastirmalari.saldiri_gelistirmesi.seviye1.aciklama);
            }else if(ork->ork_arastirmalari.saldiri_gelistirmesi.seviye2.aktiflik_durumu){
                    fprintf(dosya,"Saldırı Geliştirmesi(Seviye 2): %s\n\n",ork->ork_arastirmalari.saldiri_gelistirmesi.seviye2.aciklama);
            }else if(ork->ork_arastirmalari.saldiri_gelistirmesi.seviye3.aktiflik_durumu){
                    fprintf(dosya,"Saldırı Geliştirmesi(Seviye 3): %s\n\n",ork->ork_arastirmalari.saldiri_gelistirmesi.seviye3.aciklama);
            }
    }else if(ork->ork_arastirmalari.savunma_ustaligi.arturu_aktiflik){
            if(ork->ork_arastirmalari.savunma_ustaligi.seviye1.aktiflik_durumu){
                    fprintf(dosya,"Savunma Ustalığı (Seviye 1): %s\n\n",ork->ork_arastirmalari.savunma_ustaligi.seviye1.aciklama);
            }else if(ork->ork_arastirmalari.savunma_ustaligi.seviye2.aktiflik_durumu){
                    fprintf(dosya,"Savunma Ustalığı (Seviye 2): %s\n\n",ork->ork_arastirmalari.savunma_ustaligi.seviye2.aciklama);
            }else if(ork->ork_arastirmalari.savunma_ustaligi.seviye3.aktiflik_durumu){
                    fprintf(dosya,"Savunma Ustalığı (Seviye 3): %s\n\n",ork->ork_arastirmalari.savunma_ustaligi.seviye3.aciklama);
            }
    }else if(ork->ork_arastirmalari.elit_egitim.arturu_aktiflik){
            if(ork->ork_arastirmalari.elit_egitim.seviye1.aktiflik_durumu){
                    fprintf(dosya,"Elit Eğitim (Seviye 1): %s\n\n",ork->ork_arastirmalari.elit_egitim.seviye1.aciklama);
            }else if(ork->ork_arastirmalari.elit_egitim.seviye2.aktiflik_durumu){
                    fprintf(dosya,"Elit Eğitim (Seviye 2): %s\n\n",ork->ork_arastirmalari.elit_egitim.seviye2.aciklama);
            }else if(ork->ork_arastirmalari.elit_egitim.seviye3.aktiflik_durumu){
                    fprintf(dosya,"Elit Eğitim (Seviye 3): %s\n\n",ork->ork_arastirmalari.elit_egitim.seviye3.aciklama);
            }
    }else if(ork->ork_arastirmalari.kusatma_ustaligi.arturu_aktiflik){
            if(ork->ork_arastirmalari.kusatma_ustaligi.seviye1.aktiflik_durumu){
                    fprintf(dosya,"Kuşatma Ustalığı (Seviye 1): %s\n\n",ork->ork_arastirmalari.kusatma_ustaligi.seviye1.aciklama);
            }else if(ork->ork_arastirmalari.kusatma_ustaligi.seviye2.aktiflik_durumu){
                    fprintf(dosya,"Kuşatma Ustalığı (Seviye 2): %s\n\n",ork->ork_arastirmalari.kusatma_ustaligi.seviye2.aciklama);
            }else if(ork->ork_arastirmalari.kusatma_ustaligi.seviye3.aktiflik_durumu){
                    fprintf(dosya,"Kuşatma Ustalığı (Seviye 3): %s\n\n",ork->ork_arastirmalari.kusatma_ustaligi.seviye3.aciklama);
            }
        }

}



int main(int argc, char* argv[]) {

    FILE *dosya1 ;

    if((dosya1 = fopen("C:\\Users\\user\\Desktop\\json\\unit_types.json","r"))==NULL)
    {

        printf("Dosya acilamadi!");
        return 1 ;
    }


    struct taraf1 insan_tarafi ;
    struct taraf2 orklar ;
    ilk_deger_atama(&insan_tarafi,&orklar);

    birimleri_ayristir(dosya1,&insan_tarafi,&orklar);
    fclose(dosya1);

    FILE *dosya2 ;

    if((dosya2 = fopen("C:\\Users\\user\\Desktop\\json\\heroes.json","r"))==NULL)
    {

        printf("Dosya acilamadi!");
        return 1 ;
    }


    heros_deger_atama(dosya2,&insan_tarafi,&orklar);


    fclose(dosya2);


    FILE *dosya3 ;

    if((dosya3 = fopen("C:\\Users\\user\\Desktop\\json\\creatures.json","r"))==NULL)
    {

        printf("Dosya acilamadi!");
        return 1 ;
    }


    creatures_deger_atama(dosya3,&insan_tarafi,&orklar) ;

    fclose(dosya3);

    FILE *dosya4 ;

    if((dosya4 = fopen("C:\\Users\\user\\Desktop\\json\\research.json","r"))==NULL)
    {

        printf("Dosya acilamadi!");
        return 1 ;
    }

    arastirma_seviye_ozellik_tanimlama(dosya4,&insan_tarafi,&orklar);

    fclose(dosya4);

    FILE *dosya5 ;

    if((dosya5 = fopen("C:\\Users\\user\\Desktop\\savas_sim.txt","w"))== NULL){

        printf("Dosya acilamadi!");
        return 1 ;
    }


    senaryo_veri_cekme(&insan_tarafi, &orklar);
    savas_ilk_deger_atama(&insan_tarafi,&orklar);
    yazdir(dosya5,&insan_tarafi,&orklar);
    kritik_sansi_degistir(&insan_tarafi, &orklar);


    struct hero *herolar[] = {&insan_tarafi.Alparslan,&insan_tarafi.Fatih_Sultan_Mehmet,&insan_tarafi.Yavuz_Sultan_Selim,&insan_tarafi.Tugrul_Bey,&insan_tarafi.Mete_Han,&orklar.Goruk_Vahsi,&orklar.Thruk_Kemikkiran,&orklar.Vrog_Kafakiran,&orklar.Ugar_Zalim};
    int hero_sayisi = 9 ;
    kahraman_etkisi_uygula(herolar,&insan_tarafi,&orklar,hero_sayisi);

    struct creature *creatureslar[] = {&insan_tarafi.Ejderha,&insan_tarafi.Agri_Dagi_Devleri,&insan_tarafi.Tepegoz,&insan_tarafi.Karakurt,&insan_tarafi.Samur,&orklar.Kara_Troll,&orklar.Golge_Kurtlari,&orklar.Camur_Devleri,&orklar.Ates_Iblisi,&orklar.Makrog_Savas_Beyi,&orklar.Buz_Devleri};
    int creature_sayisi = 11 ;
    canavar_etkisi_uygula(creatureslar,&insan_tarafi,&orklar,creature_sayisi);


    // Önceki asker sayılarının tutulması (kayıp hesaplamak için)
    int prev_piyade_sayi = insan_tarafi.piyadeler.sayi;
    int prev_okcu_sayi = insan_tarafi.okcular.sayi;
    int prev_suvari_sayi = insan_tarafi.suvariler.sayi;
    int prev_kusatma_sayi = insan_tarafi.kusatma_makineleri.sayi;

    int prev_ork_dovuscu_sayi = orklar.ork_dovusculeri.sayi;
    int prev_mizrakci_sayi = orklar.mizrakcilar.sayi;
    int prev_varg_sayi = orklar.varg_binicileri.sayi;
    int prev_trol_sayi = orklar.troller.sayi;

    arastirma_etkisi_uygula(&insan_tarafi,&orklar);


    // SDL'yi başlatma
    if (!initSDL()) {
        printf("SDL başlatılamadı.\n");
        return 1;
    }


    // Birimlerin ilk sayısını kaydetme
    insan_tarafi.piyadeler.ilk_sayi = insan_tarafi.piyadeler.sayi;
    insan_tarafi.okcular.ilk_sayi = insan_tarafi.okcular.sayi;
    insan_tarafi.suvariler.ilk_sayi = insan_tarafi.suvariler.sayi;
    insan_tarafi.kusatma_makineleri.ilk_sayi = insan_tarafi.kusatma_makineleri.sayi;

    orklar.ork_dovusculeri.ilk_sayi = orklar.ork_dovusculeri.sayi;
    orklar.mizrakcilar.ilk_sayi = orklar.mizrakcilar.sayi;
    orklar.varg_binicileri.ilk_sayi = orklar.varg_binicileri.sayi;
    orklar.troller.ilk_sayi = orklar.troller.sayi;



    // Birimleri ızgaraya yerleştirme
    placeUnitsOnGrid(&insan_tarafi, &orklar);
    drawGrid();

    SDL_Event e;
    int quit = 0;
    int tur = 1;
    int savas_devam_ediyor = 1 ;
    // Savaş döngüsü
    while (!quit && savas_devam_ediyor) {


        printf("\n=== Tur: %d ===\n", tur);


        yorgunluk_hesapla(&insan_tarafi, &orklar, tur);


        kritik_vurus_hesapla(&insan_tarafi.piyadeler);
        kritik_vurus_hesapla(&insan_tarafi.okcular);
        kritik_vurus_hesapla(&insan_tarafi.suvariler);
        kritik_vurus_hesapla(&insan_tarafi.kusatma_makineleri);

        kritik_vurus_hesapla(&orklar.ork_dovusculeri);
        kritik_vurus_hesapla(&orklar.mizrakcilar);
        kritik_vurus_hesapla(&orklar.varg_binicileri);
        kritik_vurus_hesapla(&orklar.troller);


        saldiri_gucu_hesapla(&insan_tarafi, &orklar);
        savunma_gucu_hesapla(&insan_tarafi, &orklar);




        if(tur % 2 == 1){


        fprintf(dosya5,"--------------------------------------------\nAdım %d:İnsan İmparatorluğu'nun Saldırısı\n--------------------------------------------\n 1. Saldırı Gücü Hesaplaması(Araştırma ve Canavar Etkileri Dahil)\n  ",tur);
        fprintf(dosya5,"Piyadelerin toplam saldırı gücü: %.2lf\n Okçuların toplam saldırı gücü: %.2lf\n Süvarilerin toplam saldırı gücü: %.2lf\n Kuşatma Makinelerinin toplam saldırı gücü: %.2lf\n",insan_tarafi.piyadeler.toplam_saldiri_gucu,insan_tarafi.okcular.toplam_saldiri_gucu,insan_tarafi.suvariler.toplam_saldiri_gucu,insan_tarafi.kusatma_makineleri.toplam_saldiri_gucu);


        double insan_saldiri_gucu = insan_tarafi.piyadeler.toplam_saldiri_gucu +
                                    insan_tarafi.okcular.toplam_saldiri_gucu +
                                    insan_tarafi.suvariler.toplam_saldiri_gucu +
                                    insan_tarafi.kusatma_makineleri.toplam_saldiri_gucu;

        fprintf(dosya5,"Toplam Saldırı Gücü(İnsan İmparatorluğu için)\n %.1lf(Piyadeler) + %.1lf (Okçular) + %.1lf (Süvariler) + %.1lf (Kuşatma Makineleri) = %.2lf\n ",insan_tarafi.piyadeler.toplam_saldiri_gucu,insan_tarafi.okcular.toplam_saldiri_gucu,insan_tarafi.suvariler.toplam_saldiri_gucu,insan_tarafi.kusatma_makineleri.toplam_saldiri_gucu,insan_saldiri_gucu);
        double ork_savunma_gucu = orklar.ork_dovusculeri.toplam_savunma_gucu +
                                  orklar.mizrakcilar.toplam_savunma_gucu +
                                  orklar.varg_binicileri.toplam_savunma_gucu +
                                  orklar.troller.toplam_savunma_gucu;


        double fp_orkdovusculeri_toplam_savunma_gucu = orklar.ork_dovusculeri.toplam_savunma_gucu  ;
        double fp_mizrakcilar_toplam_savunma_gucu = orklar.mizrakcilar.toplam_savunma_gucu  ;
        double fp_vargbinicileri_toplam_savunma_gucu = orklar.varg_binicileri.toplam_savunma_gucu  ;
        double fp_troller_toplam_savunma_gucu = orklar.troller.toplam_savunma_gucu  ;



        fprintf(dosya5,"2.Savunma Gücü Hesaplama(Ork Lejyonu):\n Ork Dövüşçülerinin toplam savunma gücü: %.2lf\n Mızrakçıların toplam savunma gücü: %.2lf\n Varg Binicilerinin toplam savunma gücü: %.2lf\n Trollerin toplam savunma gücü: %.2lf\n",orklar.ork_dovusculeri.toplam_savunma_gucu,orklar.mizrakcilar.toplam_savunma_gucu,orklar.varg_binicileri.toplam_savunma_gucu,orklar.troller.toplam_savunma_gucu);
        fprintf(dosya5,"Toplam Savunma Gücü(Ork Lejyonu):\n %.1lf (Ork Dövüşçüleri) + %.1lf (Mızrakçılar) + %.1lf (Varg Binicileri) + %.1lf (Troller) = %.2lf\n",orklar.ork_dovusculeri.toplam_savunma_gucu,orklar.mizrakcilar.toplam_savunma_gucu,orklar.varg_binicileri.toplam_savunma_gucu,orklar.troller.toplam_savunma_gucu,ork_savunma_gucu);
        double net_hasar = net_hasar_hesapla(insan_saldiri_gucu, ork_savunma_gucu);
        fprintf(dosya5,"3.Net Hasar Hesaplaması:\n Net Hasar Formülü: Net Hasar = Toplam Saldırı Gücü x (1-(Toplam Savunma Gücü/Toplam Saldırı Gücü))\n Net Hasar = %.2lf x (1-(%.2lf/%.2lf)) = %.2lf\n",insan_saldiri_gucu,ork_savunma_gucu,insan_saldiri_gucu,net_hasar);


        struct Birim *ork_birimleri[] = {&orklar.ork_dovusculeri, &orklar.mizrakcilar,
                                         &orklar.varg_binicileri, &orklar.troller};
        int ork_birim_sayisi = 4;
        double birim_hasarlari[4] = {0.00,0.00,0.00,0.00};
        double birim_oranlari[4] = {0.00,0.00,0.00,0.00};
        hasar_dagilimi(ork_birimleri, ork_birim_sayisi, net_hasar,birim_hasarlari,birim_oranlari);


        int olen_ork_dovuscu = prev_ork_dovuscu_sayi - orklar.ork_dovusculeri.sayi;
        int olen_mizrakci = prev_mizrakci_sayi - orklar.mizrakcilar.sayi;
        int olen_varg_binici = prev_varg_sayi - orklar.varg_binicileri.sayi;
        int olen_trol = prev_trol_sayi - orklar.troller.sayi;

        fprintf(dosya5,"4.Orantılı Hasar Dağılımı:Ork Lejyonu'na %.2lf hasar orantılı olarak dağıtılır:\n",net_hasar);
        fprintf(dosya5," Ork Dövüşçüleri: %.2lf / %.2lf = %.2lf --> %.2lf x %.2lf = %.2lf hasar\n ",fp_orkdovusculeri_toplam_savunma_gucu,ork_savunma_gucu,birim_oranlari[0],net_hasar,birim_oranlari[0],birim_hasarlari[0]);
        fprintf(dosya5," Kayıp: %.2lf / %d = %d\n",birim_hasarlari[0],orklar.ork_dovusculeri.saglik,olen_ork_dovuscu);
        fprintf(dosya5," Mızrakçılar: %.2lf / %.2lf = %.2lf --> %.2lf x %.2lf = %.2lf hasar\n",fp_mizrakcilar_toplam_savunma_gucu,ork_savunma_gucu,birim_oranlari[1],net_hasar,birim_oranlari[1],birim_hasarlari[1]);
        fprintf(dosya5," Kayıp: %.2lf / %d = %d\n",birim_hasarlari[1],orklar.mizrakcilar.saglik,olen_mizrakci) ;
        fprintf(dosya5," Varg Binicileri: %.2lf / %.2lf = %.2lf --> %.2lf x %.2lf = %.2lf hasar\n ",fp_vargbinicileri_toplam_savunma_gucu,ork_savunma_gucu,birim_oranlari[2],net_hasar,birim_oranlari[2],birim_hasarlari[2]);
        fprintf(dosya5," Kayıp: %.2lf / %d = %d\n",birim_hasarlari[2],orklar.varg_binicileri.saglik,olen_varg_binici);
        fprintf(dosya5," Troller: %.2lf / %.2lf = %.2lf --> %.2lf x %.2lf = %.2lf hasar\n",fp_vargbinicileri_toplam_savunma_gucu,ork_savunma_gucu,birim_oranlari[3],net_hasar,birim_oranlari[3],birim_hasarlari[3]);
        fprintf(dosya5," Kayıp: %.2lf / %d = %d\n",birim_hasarlari[3],orklar.troller.saglik,olen_trol);

        printf("Ork Kayiplari:\n");
        printf("  Ork Dovusculeri: %d\n", olen_ork_dovuscu);
        printf("  Mizrakcilar: %d\n", olen_mizrakci);
        printf("  Varg Binicileri: %d\n", olen_varg_binici);
        printf("  Troller: %d\n", olen_trol);


        prev_ork_dovuscu_sayi = orklar.ork_dovusculeri.sayi;
        prev_mizrakci_sayi = orklar.mizrakcilar.sayi;
        prev_varg_sayi = orklar.varg_binicileri.sayi;
        prev_trol_sayi = orklar.troller.sayi;

        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = 1;
            }
        }

        }else{


        fprintf(dosya5,"--------------------------------------------\nAdım %d:Ork Lejyonu'nun Saldırısı\n--------------------------------------------\n 1. Saldırı Gücü Hesaplaması(Araştırma ve Canavar Etkileri Dahil)\n  ",tur);
        fprintf(dosya5,"Ork Dövüşçülerinin toplam saldırı gücü: %.2lf\n Mızrakçıların toplam saldırı gücü: %.2lf\n Varg Binicilerinin toplam saldırı gücü: %.2lf\n Trollerin toplam saldırı gücü: %.2lf\n",orklar.ork_dovusculeri.toplam_saldiri_gucu,orklar.mizrakcilar.toplam_saldiri_gucu,orklar.varg_binicileri.toplam_saldiri_gucu,orklar.troller.toplam_saldiri_gucu);

        double ork_saldiri_gucu = orklar.ork_dovusculeri.toplam_saldiri_gucu +
                                  orklar.mizrakcilar.toplam_saldiri_gucu +
                                  orklar.varg_binicileri.toplam_saldiri_gucu +
                                  orklar.troller.toplam_saldiri_gucu;

        fprintf(dosya5,"Toplam Saldırı Gücü(Ork Lejyonu için)\n %.1lf(Ork Dövüşçüleri) + %.1lf (Mızrakçılar) + %.1lf (Varg Binicileri) + %.1lf (Troller) = %.2lf\n ",orklar.ork_dovusculeri.toplam_saldiri_gucu,orklar.mizrakcilar.toplam_saldiri_gucu,orklar.varg_binicileri.toplam_saldiri_gucu,orklar.troller.toplam_saldiri_gucu,ork_saldiri_gucu);

        double insan_savunma_gucu = insan_tarafi.piyadeler.toplam_savunma_gucu +
                                    insan_tarafi.okcular.toplam_savunma_gucu +
                                    insan_tarafi.suvariler.toplam_savunma_gucu +
                                    insan_tarafi.kusatma_makineleri.toplam_savunma_gucu;

        double fp_piyadeler_toplam_savunma_gucu = insan_tarafi.piyadeler.toplam_savunma_gucu ;
        double fp_okcular_toplam_savunma_gucu = insan_tarafi.okcular.toplam_savunma_gucu ;
        double fp_suvariler_toplam_savunma_gucu = insan_tarafi.suvariler.toplam_savunma_gucu ;
        double fp_kusatma_toplam_savunma_gucu = insan_tarafi.kusatma_makineleri.toplam_savunma_gucu ;


        fprintf(dosya5,"2.Savunma Gücü Hesaplama(İnsan İmparatorluğu):\n Piyadelerin toplam savunma gücü: %.2lf\n Okçuların toplam savunma gücü: %.2lf\n Suvarilerin toplam savunma gücü: %.2lf\n Kuşatma Makinelerinin toplam savunma gücü: %.2lf\n",insan_tarafi.piyadeler.toplam_savunma_gucu,insan_tarafi.okcular.toplam_savunma_gucu,insan_tarafi.suvariler.toplam_savunma_gucu,insan_tarafi.kusatma_makineleri.toplam_savunma_gucu);
        fprintf(dosya5,"Toplam Savunma Gücü(İnsan İmparatorluğu):\n %.1lf (Piyadeler) + %.1lf (Okçular) + %.1lf (Suvariler) + %.1lf (Kuşatma Makineleri) = %.2lf\n",insan_tarafi.piyadeler.toplam_savunma_gucu,insan_tarafi.okcular.toplam_savunma_gucu,insan_tarafi.suvariler.toplam_savunma_gucu,insan_tarafi.kusatma_makineleri.toplam_savunma_gucu,insan_savunma_gucu);

        double net_hasar1 = net_hasar_hesapla(ork_saldiri_gucu, insan_savunma_gucu);
        fprintf(dosya5,"3.Net Hasar Hesaplaması:\n Net Hasar Formülü: Net Hasar = Toplam Saldırı Gücü x (1-(Toplam Savunma Gücü/Toplam Saldırı Gücü))\n Net hasar = %.2lf x (1-(%.2lf/%.2lf)) = %.2lf\n",ork_saldiri_gucu,insan_savunma_gucu,ork_saldiri_gucu,net_hasar1);
        struct Birim *insan_birimleri[] = {&insan_tarafi.piyadeler, &insan_tarafi.okcular,
                                           &insan_tarafi.suvariler, &insan_tarafi.kusatma_makineleri};
        int insan_birim_sayisi = 4;
        double oran_birim[4] = {0.00,0.00,0.00,00};
        double hasar_birim[4] = {0.00,0.00,0.00,0.00};

        hasar_dagilimi(insan_birimleri, insan_birim_sayisi, net_hasar1,hasar_birim,oran_birim);


        int olen_piyade = prev_piyade_sayi - insan_tarafi.piyadeler.sayi;
        int olen_okcu = prev_okcu_sayi - insan_tarafi.okcular.sayi;
        int olen_suvari = prev_suvari_sayi - insan_tarafi.suvariler.sayi;
        int olen_kusatma = prev_kusatma_sayi - insan_tarafi.kusatma_makineleri.sayi;

        fprintf(dosya5,"4.Orantılı Hasar Dağılımı:İnsan İmparatorluğu'na %.2lf hasar orantılı olarak dağıtılır:\n",net_hasar1);
        fprintf(dosya5," Piyadeler: %.2lf / %.2lf = %.2lf --> %.2lf x %.2lf = %.2lf hasar\n ",fp_piyadeler_toplam_savunma_gucu,insan_savunma_gucu,oran_birim[0],net_hasar1,oran_birim[0],hasar_birim[0]);
        fprintf(dosya5," Kayıp: %.2lf / %d = %d\n",hasar_birim[0],insan_tarafi.piyadeler.saglik,olen_piyade);
        fprintf(dosya5," Okçular: %.2lf / %.2lf = %.2lf --> %.2lf x %.2lf = %.2lf hasar\n",fp_okcular_toplam_savunma_gucu,insan_savunma_gucu,oran_birim[1],net_hasar1,oran_birim[1],hasar_birim[1]);
        fprintf(dosya5," Kayıp: %.2lf / %d = %d\n",hasar_birim[1],insan_tarafi.okcular.saglik,olen_okcu);
        fprintf(dosya5," Süvariler: %.2lf / %.2lf = %.2lf --> %.2lf x %.2lf = %.2lf hasar\n ",fp_suvariler_toplam_savunma_gucu,insan_savunma_gucu,oran_birim[2],net_hasar1,oran_birim[2],hasar_birim[2]);
        fprintf(dosya5," Kayıp: %.2lf / %d = %d\n",hasar_birim[2],insan_tarafi.suvariler.saglik,olen_suvari);
        fprintf(dosya5," Kuşatma Makineleri: %.2lf / %.2lf = %.2lf --> %.2lf x %.2lf = %.2lf hasar\n",fp_kusatma_toplam_savunma_gucu,insan_savunma_gucu,oran_birim[3],net_hasar1,oran_birim[3],hasar_birim[3]);
        fprintf(dosya5," Kayıp: %.2lf / %d = %d\n",hasar_birim[3],insan_tarafi.kusatma_makineleri.saglik,olen_kusatma);


        printf("Insan Kayiplari:\n");
        printf("  Piyadeler: %d\n", olen_piyade);
        printf("  Okcular: %d\n", olen_okcu);
        printf("  Suvariler: %d\n", olen_suvari);
        printf("  Kusatma Makineleri: %d\n", olen_kusatma);


        prev_piyade_sayi = insan_tarafi.piyadeler.sayi;
        prev_okcu_sayi = insan_tarafi.okcular.sayi;
        prev_suvari_sayi = insan_tarafi.suvariler.sayi;
        prev_kusatma_sayi = insan_tarafi.kusatma_makineleri.sayi;

        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = 1;
            }
        }

        }

        updateHealthPercentages(&insan_tarafi, &orklar);


        drawGrid();

        SDL_Delay(1000);



        if (orklar.ork_dovusculeri.sayi == 0 && orklar.mizrakcilar.sayi == 0 &&
            orklar.varg_binicileri.sayi == 0 && orklar.troller.sayi == 0) {
            printf("\nInsanlar savasi kazandi!\n");
            fprintf(dosya5,"\nOrk Lejyonu tamamen yok edildi.İnsan İmparatorluğu kazandi!\n\n------------İnsan İmparatorluğu kazandi!-----------");
            savas_devam_ediyor = 0;

        }
        if (insan_tarafi.piyadeler.sayi == 0 && insan_tarafi.okcular.sayi == 0 &&
            insan_tarafi.suvariler.sayi == 0 && insan_tarafi.kusatma_makineleri.sayi == 0) {
            printf("\nOrklar savasi kazandi!\n");
            fprintf(dosya5,"\nİnsan İmparatorluğu tamamen yok edildi.Ork Lejyonu kazandi!\n\n------------Ork Lejyonu kazandi!-----------");
            savas_devam_ediyor = 0;
        }

        tur++;
    }

    printf("\n=== Savas Sonu ===\n");
    printf("Toplam Tur: %d\n", tur - 1);


    printf("Insan Tarafi Kalan Birimler:\n");
    printf("  Piyadeler: %d\n", insan_tarafi.piyadeler.sayi);
    printf("  Okcular: %d\n", insan_tarafi.okcular.sayi);
    printf("  Suvariler: %d\n", insan_tarafi.suvariler.sayi);
    printf("  Kusatma Makineleri: %d\n", insan_tarafi.kusatma_makineleri.sayi);

    printf("Ork Tarafi Kalan Birimler:\n");
    printf("  Ork Dovusculeri: %d\n", orklar.ork_dovusculeri.sayi);
    printf("  Mizrakcilar: %d\n", orklar.mizrakcilar.sayi);
    printf("  Varg Binicileri: %d\n", orklar.varg_binicileri.sayi);
    printf("  Troller: %d\n", orklar.troller.sayi);



    placeUnitsOnGrid(&insan_tarafi, &orklar);
    drawGrid();


    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = 1;
            }
        }
        SDL_Delay(100);
    }


    closeSDL();




    fclose(dosya5);

    return 0;
}
