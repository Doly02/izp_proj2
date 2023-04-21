/**
 * Kostra programu pro 2. projekt IZP 2022/23
 *
 * Jednoducha shlukova analyza: 2D nejblizsi soused.
 * Single linkage
 */
/*
 vypracoval:
 jméno: Tomáš Dolák
 login: xdolak09
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h> // sqrtf
#include <limits.h> // INT_MAX

/*****************************************************************
 * Ladici makra. Vypnout jejich efekt lze definici makra
 * NDEBUG, napr.:
 *   a) pri prekladu argumentem prekladaci -DNDEBUG
 *   b) v souboru (na radek pred #include <assert.h>
 *      #define NDEBUG
 */
#ifdef NDEBUG
#define debug(s)
#define dfmt(s, ...)
#define dint(i)
#define dfloat(f)
#else

// vypise ladici retezec
#define debug(s) printf("- %s\n", s)

// vypise formatovany ladici vystup - pouziti podobne jako printf
#define dfmt(s, ...) printf(" - "__FILE__":%u: "s"\n",__LINE__,__VA_ARGS__)

// vypise ladici informaci o promenne - pouziti dint(identifikator_promenne)
#define dint(i) printf(" - " __FILE__ ":%u: " #i " = %d\n", __LINE__, i)

// vypise ladici informaci o promenne typu float - pouziti
// dfloat(identifikator_promenne)
#define dfloat(f) printf(" - " __FILE__ ":%u: " #f " = %g\n", __LINE__, f)

#endif

/*****************************************************************
 * Deklarace potrebnych datovych typu:
 *
 * TYTO DEKLARACE NEMENTE
 *
 *   struct obj_t - struktura objektu: identifikator a souradnice
 *   struct cluster_t - shluk objektu:
 *      pocet objektu ve shluku,
 *      kapacita shluku (pocet objektu, pro ktere je rezervovano
 *          misto v poli),
 *      ukazatel na pole shluku.
 */

struct obj_t {
    int id;
    float x;
    float y;
};

struct cluster_t {
    int size;
    int capacity;
    struct obj_t *obj;
};

/*****************************************************************
 * Deklarace potrebnych funkci.
 *
 * PROTOTYPY FUNKCI NEMENTE
 *
 * IMPLEMENTUJTE POUZE FUNKCE NA MISTECH OZNACENYCH 'TODO'
 *
 */


/*
 Inicializace shluku 'c'. Alokuje pamet pro cap objektu (kapacitu).
 Ukazatel NULL u pole objektu znamena kapacitu 0.
*/

void init_cluster(struct cluster_t *c, int cap)
{
    assert(c != NULL);
    assert(cap >= 0);
    c->size = 0;
    c->capacity = cap;

    if (cap != 0) {
        c->obj = malloc(cap * sizeof(struct obj_t));
        if(c->obj == NULL)
        {
            fprintf(stderr, "Allocation was not possible\n");
            return;
        }
    }
    
}


/*
 Odstraneni vsech objektu shluku a inicializace na prazdny shluk.
 */
void clear_cluster(struct cluster_t *c)
{
    assert(c != NULL);
    free(c->obj);
    
    init_cluster(c, 0);
}


/*
 funkce nejprve uvolni pamet pro objekty shluku a pote uvolni pamet samotnoho shluku
 */
void clear_clusters(struct cluster_t *carr, const int narr)
{
    assert(carr);
    assert(narr >= 0);

    for (int i = 0; i < narr; i++) {
        clear_cluster(&carr[i]);
    }
    free(carr);
}


/*
 pomocna fce, na kontrolu unikatnosti ID, v pripade ze fce najde dve stejna ID vrací hodnotu mínus -1
 */

int id_controller(struct cluster_t **c, int size)
{
    for (int i = 0; i < size; i++)
    {
        for (int j = i+1; j < size; j++)
        {
            if ((*c)[i].obj[0].id == (*c)[j].obj[0].id)
                return -1;
        }
    }
    return 1;
}


/*
 pomocna fce, testujici zda souradnice jsou cela cisla
 */
int integer_testing(float x, float y)
{
    
    if (x == (int)x )
    {
        if (y == (int)y )
            return 0;
    }

    return -1;
}


/// Chunk of cluster objects. Value recommended for reallocation.
const int CLUSTER_CHUNK = 10;


/*
 Zmena kapacity shluku 'c' na kapacitu 'new_cap'.
 */
struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap)
{
    // TUTO FUNKCI NEMENTE
    assert(c);
    assert(c->capacity >= 0);
    assert(new_cap >= 0);

    if (c->capacity >= new_cap)
        return c;

    size_t size = sizeof(struct obj_t) * new_cap;

    void *arr = realloc(c->obj, size);
    if (arr == NULL)
        return NULL;

    c->obj = (struct obj_t*)arr;
    c->capacity = new_cap;
    return c;
}


/*
 Prida objekt 'obj' na konec shluku 'c'. Rozsiri shluk, pokud se do nej objekt
 nevejde.
 */
void append_cluster(struct cluster_t *c, struct obj_t obj)
{
    if (c->capacity <= c->size)
        c = resize_cluster(c, c->capacity+CLUSTER_CHUNK);
    if(c != NULL)
        c->obj[c->size++] = obj;
}



/*
 Seradi objekty ve shluku 'c' vzestupne podle jejich identifikacniho cisla.
 */
void sort_cluster(struct cluster_t *c);



/*
 Do shluku 'c1' prida objekty 'c2'. Shluk 'c1' bude v pripade nutnosti rozsiren.
 Objekty ve shluku 'c1' budou serazeny vzestupne podle identifikacniho cisla.
 Shluk 'c2' bude nezmenen.
 */
void merge_clusters(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c2 != NULL);
    
    int apppended_size_c1 = c1->size + c2->size;

    for (int i = 0; i < c2->size; i++)
    {
        append_cluster(c1, c2->obj[i]);
    }
    if (c1->size == apppended_size_c1)
        sort_cluster(c1);

}

/**********************************************************************/
/* Prace s polem shluku */


/*
 Odstrani shluk z pole shluku 'carr'. Pole shluku obsahuje 'narr' polozek
 (shluku). Shluk pro odstraneni se nachazi na indexu 'idx'. Funkce vraci novy
 pocet shluku v poli.
*/
int remove_cluster(struct cluster_t *carr, int narr, int idx)
{
    assert(idx < narr);
    assert(narr > 0);
    
    int i;
    
    //odstraneni shluku z pole shluku 'carr'
    clear_cluster(&carr[idx]);
    //novy pocet polozek
    int new_number_of_items = narr - 1;
    
    //posunuti polozek
    for (i = idx; i < new_number_of_items; i++)
    {
        carr[i] = carr[i+1];
    }
    
    return new_number_of_items;
}


/*
 Pocita Euklidovskou vzdalenost mezi dvema objekty.
 */
float obj_distance(struct obj_t *o1, struct obj_t *o2)
{
    assert(o1 != NULL);
    assert(o2 != NULL);
    
    float x_difference = (o1->x - o2->x);
    x_difference *= x_difference;
    
    float y_difference = (o1->y - o2->y);
    y_difference *= y_difference;

    float distance = 0.0;
    float final_distance = 0.0;
    distance = (x_difference + y_difference);
    final_distance = sqrtf(distance);
    
    return final_distance;

}


/*
  kontrola jednotlivých souřadnic objektu v pripade ze je nektera souradnice mensi nule nebo vetsi rovna tisici je vracena minus jednicka
*/
int cluster_location_controller(float x, float y)
{
    if (x >= 1000 || x < 0)
        return -1;
    
    if (y >= 1000 || y < 0)
        return -1;
    
    return 1;
}


/*
 Pocita vzdalenost dvou shluku. Vraci minimalni vzdalenost mezi shluky
*/
float cluster_distance(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c1->size > 0);
    assert(c2 != NULL);
    assert(c2->size > 0);

    //prvni nastavena minimalni vzdalenost
    float minimal_distance = obj_distance(&c1->obj[0], &c2->obj[0]);

    for (int i = 0; i < c1->size; i++)
        for (int j = 0; j < c2->size; j++) {
            float distance = obj_distance(&c1->obj[i], &c2->obj[j]);
            if (distance < minimal_distance)
                minimal_distance = distance;
        }

    return minimal_distance;
}


/*
 Funkce najde dva nejblizsi shluky. V poli shluku 'carr' o velikosti 'narr'
 hleda dva nejblizsi shluky. Nalezene shluky identifikuje jejich indexy v poli
 'carr'. Funkce nalezene shluky (indexy do pole 'carr') uklada do pameti na
 adresu 'c1' resp. 'c2'.
*/
void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2)
{
    assert(narr > 0);
    
    float minimal_distance = 999.0;
    float distance = 0.0;

    for (int index_c1 = 0; index_c1 < narr; index_c1++)
    {
        for (int index_c2 = 0; index_c2 < narr; index_c2++)
        {
            if((distance = cluster_distance(&carr[index_c1], &carr[index_c2])))
            {
                if(distance < minimal_distance)
                {
                    minimal_distance = distance;
                    *c1 = index_c1;
                    *c2 = index_c2;
                }
            }
        }
    }
}


// pomocna funkce pro razeni shluku
static int obj_sort_compar(const void *a, const void *b)
{
    // TUTO FUNKCI NEMENTE
    const struct obj_t *o1 = (const struct obj_t *)a;
    const struct obj_t *o2 = (const struct obj_t *)b;
    if (o1->id < o2->id) return -1;
    if (o1->id > o2->id) return 1;
    return 0;
}


/*
 Razeni objektu ve shluku vzestupne podle jejich identifikatoru.
*/
void sort_cluster(struct cluster_t *c)
{
    // TUTO FUNKCI NEMENTE
    qsort(c->obj, c->size, sizeof(struct obj_t), &obj_sort_compar);
}


/*
 Tisk shluku 'c' na stdout.
*/
void print_cluster(struct cluster_t *c)
{
    // TUTO FUNKCI NEMENTE
    for (int i = 0; i < c->size; i++)
    {
        if (i) putchar(' ');
        printf("%d[%g,%g]", c->obj[i].id, c->obj[i].x, c->obj[i].y);
    }
    putchar('\n');
}


/*
pomocna fce, uklada do struktury objektu ID a souradnice X,Y, v pripade hladkeho prubehu vraci hodnotu jedna
*/

int load_object(struct obj_t *object ,int object_id, float object_x, float object_y)
{
    //kontrola zda jsou souradnice cele cisla
    if(integer_testing(object_x, object_y) != 0)
    {
        fprintf(stderr, "The cooridinates are not an integer\n");
        return -1;
    }
    //kontrola zda jsou souradnice v intervalu (0,1000)
    if(cluster_location_controller(object_x,object_y) == -1)
    {
        fprintf(stderr, "Bad location\n");
        return -1;
    }

    //prirazeni do objektu
    object->id = object_id;
    object->x = object_x;
    object->y = object_y;
    
    
    
    return 1;
}


/*
 Ze souboru 'filename' nacte objekty. Pro kazdy objekt vytvori shluk a ulozi
 jej do pole shluku. Alokuje prostor pro pole vsech shluku a ukazatel na prvni
 polozku pole (ukalazatel na prvni shluk v alokovanem poli) ulozi do pameti,
 kam se odkazuje parametr 'arr'. Funkce vraci pocet nactenych objektu (shluku).
 V pripade nejake chyby uklada do pameti, kam se odkazuje 'arr', hodnotu NULL.
*/
int load_clusters(char *filename, struct cluster_t **arr)
{
    assert(arr != NULL);
    
    *arr = NULL;
    
      int count = 0; //overuje zda bylo nacteno tolik clusteru kolik bylo receno

      

      int object_id = 0;
      float object_x = 0.0;
      float object_y = 0.0;
      char koncovy_charakter = '\0';
      
      struct obj_t object;
      struct cluster_t *cluster = NULL;
      
      
      
      FILE *file;
      file = fopen(filename, "r");
      if (file == NULL)
      {
          fprintf(stderr, "File not found.\n");
          return -1;
      }
    //nacteni poctu polozek
      fscanf(file, "count=%d", &count);
      
    //malokovani prostoru pro jednotlive clustery
    cluster = malloc(count * sizeof(struct cluster_t));
    if( cluster == NULL )
    {
        fprintf(stderr, "No memory for allocation\n");
        return -1;
    }
    
    //malokovani prostoru pro objekty clusteru
    for (int i = 0; i < count; i++)
        init_cluster(&cluster[i], 1);


    for (int i = 0; i < count; i++)
      {
          if(fscanf(file, "%d %f %f%c", &object_id, &object_x, &object_y,&koncovy_charakter) != 4)
          {
              fprintf(stderr, "Random text\n");
              return -1;
          }
          
          //overeni zda je na jednom radku pouze jeden objekt
          if(koncovy_charakter != '\n')
          {
              fprintf(stderr, "More objects on one line\n");
              return -1;
          }
          //nacitani hodnot do objektu
          if(load_object(&object, object_id, object_x, object_y) == -1)
          {
              return -1;
          }
        //pridani objectu do ukazatele
          append_cluster(&cluster[i], object);
          
          
      }
    
    //kontrola ID vsech objektu
    if(id_controller(&cluster, count) == -1)
    {
        fprintf(stderr, "ID is not unique\n");
        clear_clusters(cluster, count);
        return -1;
    }

    //nahrani adresy cluster do *arr
      *arr = cluster;
      return count;
      
}


/*
 Tisk pole shluku. Parametr 'carr' je ukazatel na prvni polozku (shluk).
 Tiskne se prvnich 'narr' shluku.
*/
void print_clusters(struct cluster_t *carr, int narr)
{
    printf("Clusters:\n");
    for (int i = 0; i < narr; i++)
    {
        printf("cluster %d: ", i);
        print_cluster(&carr[i]);
    }
}


/*
 funkce kontrolujici argument, zda obsahuje pouze cisla, v pripade ze neobsahuje, vraci -1
 */
int argument_controller(char argument[])
{
    char *p_argument = argument;
    while (*p_argument != '\0')
    {
        if (*p_argument<'0' || *p_argument>'9')
        {
            return -1;
        }
        p_argument++;
    }
    return 1;
}


/*
 prejmenovat tuhle funkci!
 funkce dostava pointer na shluky, aktualni pocet shluku a pozadovany pocet, fce spojuje nejblizsi dva shluky dokud se aktualni pocet shluku nerovnana pozadovanemu, v pripade ze je pozadovany pocet vetsi nez aktualni vraci -1, jinak vraci aktualni pocet
 */
int cluster_reduce(struct cluster_t *cluster, int actual_number, const int required_number)
{
    int first_cluster;
    int second_cluster;
    if (required_number < actual_number)
    {
        while (required_number < actual_number)
        {
            find_neighbours(cluster, actual_number, &first_cluster, &second_cluster);
            merge_clusters(&cluster[first_cluster], &cluster[second_cluster]);
            remove_cluster(cluster, actual_number, second_cluster);
            actual_number--;

        }
        
    }
    if(actual_number < required_number)
    {
        fprintf(stderr, "You want more clusters than is possible\n");
        return -1;
    }
    return actual_number;
}


int main(int argc, char *argv[])
{
    if (argc == 1) //spatna moznost
    {
        fprintf(stderr, "no arguments\n");
        return -1;
    }
    
    if (argc == 2) //první dobra moznost (jako argument pouze soubor -> vse do jednoho clusteru)
    {
        int no_items = 0; //pocet objektu
        struct cluster_t *clusters;
        
        if((no_items = load_clusters(argv[1], &clusters)) == -1)
        {
            fprintf(stderr, "That is not a file with propriet format\n");
            return -1;
        }
        
        int size;
        //v tomto pripade je jako default prednastaveno ze se vse hodi do jednoho clusteru
        size = cluster_reduce(clusters, no_items, 1);
        
        if (size == -1)
            return -1;
        
        print_clusters(clusters, size);
        //finalni uvolneni
        clear_clusters(clusters, size);
        return 0;
    }
    
    if (argc == 3) //druha dobra moznost (argumenty soubor a pozadovany pocet clusteru)
    {
        
        int no_items = 0; //kolik je objektu
        if (argument_controller(argv[2]) == -1)
        {
            fprintf(stderr, "wrong argument\n");
            return -1;
        }
        
        struct cluster_t *clusters; //deklarace
        if((no_items = load_clusters(argv[1], &clusters)) == -1)
        {
            fprintf(stderr, "That is not a file with propriet format\n");
            return -1;
        }
        //ziskani poctu finalniho poctu clusteru
        int argument = atoi(argv[2]);
        
        if (argument == 0)
        { //nejde udelat nula clusteru
            return -1;
        }
        
        int size;   //do SIZE se ulozi finalni pocet clusteru
        size = cluster_reduce(clusters, no_items, argument);
        
        if (size == -1)
            return -1;
        

        print_clusters(clusters, size);
        //finalni uvolneni
        clear_clusters(clusters, size);
    }
    
    if (argc > 3) //spatna moznost
    {
        fprintf(stderr, "To many arguments\n");
        return -1;
    }

    printf("\n");
    return 0;
}
