#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <ctype.h>
#include <algorithm>
#pragma warning(disable : 4996)

typedef unsigned int Elem_t;
#define ELEM_T "u"

const Elem_t POISON         = -1073676287;
const int    INITSIZE       = 1000,
             INCREASECOEFF  = 2,
             N_ERRORS       = 5,
             SIZE_ERROR     = 1,
             FREE_ERROR     = 2,
             TAIL_ERROR     = 3,
             HEAD_ERROR     = 4,
             DATA_ERROR     = 5,
             NULL_POINT     = 6;

#define CHECKLIST(list) /*if(ListOk(list)) ListDump(list, __FILE__, __LINE__, __PRETTY_FUNCTION__); */

struct Element {
 
    Elem_t   data;
    Element* next;
};

struct List {

    Element* Elem;
    Element* head;
    Element* tail;
    Element* free;
    int      size;
    int      errcode;
};

int ListOk(List* list) {

    if (!list)
        return NULL_POINT;

    if (list->size < 0)
        list->errcode |= (1 << (SIZE_ERROR - 1));

    if (!list->free)
        list->errcode |= (1 << (FREE_ERROR - 1));

    if (!list->tail)
        list->errcode |= (1 << (TAIL_ERROR - 1));

    if (!list->head)
        list->errcode |= (1 << (HEAD_ERROR - 1));

    if (!list->Elem)
        list->errcode |= (1 << (DATA_ERROR - 1));

    else if ( !((list->errcode >> (SIZE_ERROR - 1)) & 1) & !((list->errcode >> (FREE_ERROR - 1)) & 1) ) {
        for(Element* lf = list->free; lf != list->Elem; lf = lf->next) {
            if (lf->data != POISON || !lf->next) {
                list->errcode |= (1 << (DATA_ERROR - 1));
                break;
            }
        }
    }
    else if ( !((list->errcode >> (SIZE_ERROR - 1)) & 1) & !((list->errcode >> (TAIL_ERROR - 1)) & 1) & !((list->errcode >> (HEAD_ERROR - 1)) & 1) ) {
        for(Element* ld = list->head; ld != list->Elem; ld = ld->next) {
            if (!ld->next) {
                list->errcode |= (1 << (DATA_ERROR - 1));
                break;
            }
        }
    }

    return list->errcode;
}

bool DotWriter(const List* list) {

    assert(list);

    FILE* DotListDump = fopen("DotListDump.txt", "wb");
    if (!DotListDump) {
        printf("\n COULD NOT OPEN DUMP FILE\n");
        return 1;
    }

    fprintf(DotListDump, "digraph ListDump {\n"
                         "\"Elem[%p] \\n data: %d \\n next: %p\"", list->Elem, list->Elem->data, list->Elem);

    for(Element* ld = list->head; ld != list->Elem; ld = ld->next) {
        fprintf(DotListDump, "->\"Elem[%p] \\n data: %d \\n next: %p\"", ld, ld->data, ld->next);
        }

    fprintf(DotListDump, "\n");

    fprintf(DotListDump, "FREE");

    for(Element* lf = list->free; lf != list->Elem; lf = lf->next) {
        fprintf(DotListDump, "->\"Elem[%p] \\n data: %d \\n next: %p\"", lf, lf->data, lf->next);
        }

    fprintf(DotListDump, "\n}");

    fclose(DotListDump);

    system("dot -Tjpg -o F:\\ListDump.jpg F:\\proga\\HashTable\\DotListDump.txt");
    system("F:\\ListDump.jpg");
    return 0;
}

bool ListDump(const List* list, const char* file, int line, const char* function) {

    if (!list) {
        printf("\n ERROR: NULL POINTER TO LIST! \n");
        return 1;
    }

    //printf("\n LIST DUMP: File: %s, line: %d, function: %s \n", file, line, function);

    printf("\n List [%p] ", list);
    (list->errcode == 0) ? printf("(ok)\n\n") : printf("(ERROR)\n\n");

    for(int i = 0; i < list->size; i++)
        printf("[%2p] : %4d %3p\n", list->Elem + i, list->Elem[i].data, list->Elem[i].next);

    printf("\nhead = %p\n"
           "tail = %p\n"
           "free = %p\n"
           "size = %d\n"
           "errcode = %d " , list->head, list->tail, list->free, list->size, list->errcode);

    printf("\n");

    //DotWriter(list);

    return 0;
}

int ListIncrease(List* list) {

    CHECKLIST(list)

    Element* reserve = (Element*)realloc(list->Elem, sizeof(Element) * (list->size *= INCREASECOEFF));
    list->Elem = (reserve) ? reserve : list->Elem;

    if (!reserve) {
        list->size /= INCREASECOEFF;
        printf("Not able to increase list");
        return 1;
    }

    int ph = list->size / INCREASECOEFF;
    list->free = &list->Elem[ph];

    for( ; ph < list->size; ph++) {
         list->Elem[ph].data = POISON;
         list->Elem[ph].next = &list->Elem[(ph + 1) % list->size];
    }

    CHECKLIST(list);
    return 0;
}

/*int ListDecrease(List* list) {

    CHECKLIST(list)

    Element* reserve = (Element*)realloc(list->Elem, list->size /= INCREASECOEFF);
    list->Elem = (reserve != NULL) ? reserve : list->Elem;

    if (!reserve) {
        list->size *= INCREASECOEFF;
        printf("Not able to decrease list");
        return 1;
    }

    CHECKLIST(list);
    return 0;
}

bool ListSort(List* list) {

    CHECKLIST(list)

    if (!list->head)
        return 1;

    Element* reserve = (Element*)calloc(list->size, sizeof(Element));
    if (!reserve)
        return 1;

    reserve[0].data = POISON;

    int ph = 1;

    for( int i = list->head; i; ph++, i = list->Elem[i].next) {
        reserve[ph].data = list->Elem[i].data;
        reserve[ph].next = ((ph + 1) + list->size) % list->size;
        reserve[ph].prev = ((ph - 1) + list->size) % list->size;
    }

    reserve[ph - 1].next = 0;
    list->head = 1;
    list->tail = ph - 1;
    list->free = ph;

    for( ; ph < list->size; ph++) {
         reserve[ph].data = POISON;
         reserve[ph].next = ((ph + 1) + list->size) % list->size;
         reserve[ph].prev = -1;
    }
    free(list->Elem);
    list->Elem = reserve;

    CHECKLIST(list)
    return 0;
} */

bool ListInit(List* list) {

    assert(list != NULL);

    list->size = INITSIZE;

    list->Elem = (Element*)calloc(list->size, sizeof(Element));

    if (!list->Elem) {
        printf("COULD NOT INIT LIST");
        return 1;
    }

    list->head    = list->Elem;
    list->free    = list->Elem + 1;
    list->tail    = list->Elem;
    list->errcode = 0;

    for(int i = 1; i < list->size; i++) {
         list->Elem[i].data = POISON;
         list->Elem[i].next = list->Elem + (i + 1) % list->size;
    }

    list->Elem[0].data = POISON;
    list->Elem[0].next = list->Elem;

    CHECKLIST(list);
    return 0;
}

Element* ValueSearch(List* list, Elem_t value) {//returns physical number

    CHECKLIST(list);

    Element* ld = list->head;

    for( ; ld != list->Elem; ld = ld->next)
        if (ld->data == value)
            break;

    CHECKLIST(list);
    return (ld == list->Elem) ? NULL : ld;
    }

/*int NumberSearch(List* list, int number) {//logical number, returns physical number

    CHECKLIST(list);

    int pn = list->head;

    for(int counter = 1; counter != number && pn != 0; pn = list->Elem[pn].next, counter++);

    CHECKLIST(list);
    return pn;
} */

/*int PushPrev(List* list, Elem_t value, int number) {//physical number

    CHECKLIST(list);
                                                                   //PushNext
    int exfree = list->free;
    list->free = list->Elem[list->free].next;

    list->head = (number == list->head) ? exfree : list->head;

    list->Elem[list->Elem[number].prev].next = (list->Elem[number].prev) ? exfree : 0;

    list->Elem[exfree].data = value;
    list->Elem[exfree].prev = list->Elem[number].prev;
    list->Elem[exfree].next = number;
    list->Elem[number].prev = exfree;

    CHECKLIST(list);
    return exfree;
}

int PushNext(List* list, Elem_t value, int number) {//physical number

    CHECKLIST(list);

    int exfree = list->free;
    list->free = list->Elem[list->free].next;

    list->tail = (list->tail == number) ? exfree : list->tail;

    list->Elem[list->Elem[number].next].prev = (list->Elem[number].next) ? exfree : 0;

    list->Elem[exfree].data = value;
    list->Elem[exfree].next = list->Elem[number].next;
    list->Elem[exfree].prev = number;
    list->Elem[number].next = exfree;

    CHECKLIST(list);
    return exfree;
}*/

Element* PushLast(List* list, Elem_t value) {//physical number

    CHECKLIST(list);

    if (list->free == list->Elem)
        ListIncrease(list);

    Element* exfree = list->free;
    list->free = list->free->next;

    list->head = (list->tail == list->Elem) ? exfree : list->head;

    exfree->data     = value;
    list->tail->next = (list->tail == list->Elem) ? list->Elem : exfree;
    list->tail       = exfree;
    exfree->next     = list->Elem;

    CHECKLIST(list);
    return exfree;
}

/*int PushFirst(List* list, Elem_t value) {//physical number

    CHECKLIST(list);

    int exfree = list->free;
    list->free = list->Elem[list->free].next;

    list->tail = (list->head == 0) ? exfree : list->tail;

    list->Elem[exfree].data     = value;
    list->Elem[list->head].prev = (list->head == 0) ? 0 : exfree;
    list->Elem[exfree].next     = list->head;
    list->head                  = exfree;
    list->Elem[exfree].prev     = 0;

    CHECKLIST(list);
    return exfree;
}

int ListPush(List* list, Elem_t value, const char mode, Elem_t num = 0) {//mode = (f - first, l - last, p - prev, n - next)

    CHECKLIST(list);

    if (!list->free)
        ListIncrease(list);


    if (mode == 'f')
        return PushFirst(list, value);

    if (mode == 'l')
        return PushLast(list, value);

    int ph = ValueSearch(list, num);

    if (ph == -1) {
        printf("\nELEMENT TO PUSH IS NOT FOUND\n");
        return 0;
    }

    if (mode == 'p')
        return (ph == list->head) ? PushFirst(list, value) : PushPrev(list, value, ph);

    if (mode == 'n')
        return (ph == list->tail) ? PushLast(list, value) : PushNext(list, value, ph);


    printf("UNKNOWN MODE");
    CHECKLIST(list);
    return 0;
} */

/*int ListDeletePH(List* list, int number) {//physical number

    CHECKLIST(list);

    if (list->Elem[number].prev == -1 || !number)
        return 0;

    if (number == list->tail) {
        list->Elem[list->Elem[number].prev].next = list->Elem[number].next;
        list->tail = list->Elem[number].prev;
    }
    else if (number == list->head) {
        list->Elem[list->Elem[number].next].prev = list->Elem[number].prev;
        list->head = list->Elem[number].next;
    }
    else {
        list->Elem[list->Elem[number].prev].next = list->Elem[number].next;
        list->Elem[list->Elem[number].next].prev = list->Elem[number].prev;
    }

    list->Elem[number].data = POISON;
    list->Elem[number].next = list->free;
    list->Elem[number].prev = -1;

    list->free = number;

    CHECKLIST(list);
    return number;
}

int ListDelete(List* list, Elem_t value) {

    CHECKLIST(list);

    int ph = ValueSearch(list, value);
    if (ph == -1) {
        printf("\nELEMENT TO DELETE IS NOT FOUND\n");
        return 0;
    }

    CHECKLIST(list);
    return ListDeletePH(list, ph);
} */

bool ListKill(List* list) {

    CHECKLIST(list);

    free(list->Elem);
    list->size    = 0;
    list->head    = NULL;
    list->free    = NULL;
    list->tail    = NULL;
    list->errcode = 0;
    list->Elem    = NULL;

    return 0;
}
