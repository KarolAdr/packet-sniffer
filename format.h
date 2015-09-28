typedef struct printer {
    void(*printInData)(char*, int, int);
    void(*printOutData)(char*, int, int);
    
}printer;

printer *initPrinter(void);
void deallocPrinter(printer*); 
