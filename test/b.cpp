#include "../GWindow.h"
class GM : public GWindowRun
{
    public:
    GM(const GString& s){
    }
    void draw(){};
};
#define ID_FIRST 1000
#define ID_OUTLIST ID_FIRST
#define ID_FRAMEDOWN ID_FIRST + 1

int limit(int a, int b){
    unsigned long r = random();
    int ret;
    ret = r % (b - a + 1) + a;
    return ret;
}
#define UP 20 
GString makePlus(){
    GString a;
    int first = limit(1,UP) ;
    int second = limit(1,UP);
    while( first + second > (UP)){
        first = limit(1,UP) ;
        second = limit(1,UP);
    }
    a.format("%2d + %2d = %2d" , first, second , first + second);
    return a;
}
GString makeMinus(){
    GString a;
    int first = limit(1,UP) ;
    int second = limit(1,UP);
    int tmp;
    while( first == second){
        first = limit(1,UP);
    }
    if (first < second){
        tmp = first;
        first = second;
        second = tmp;
    }
    a.format("%2d - %2d = %2d" , first, second , first - second);
    return a;
}
GString makeMultiply(){
    GString a;
    int first = limit(1,9) ;
    int second = limit(1,9);
    a.format("%d * %d = %2d" , first, second , first * second);
    return a;
}

void fplusimpl();
void fminusimpl();
void fmultiplyimpl();
#define MAP(a,type,id) type a = (type)GgetWindowMap(id)
void fplus(const XEvent& ev){
    //GListBox * p = (GListBox*) GgetWindowMap(ID_OUTLIST);
    MAP(p,GListBox*,ID_OUTLIST);
    p->clear();
    fplusimpl();
};
void fplusimpl(){
    GListBox * p = (GListBox*) GgetWindowMap(ID_OUTLIST);
    for (int i = 0;i < 5; i++){
        p->addList(makePlus());
    }
    p->update();
}
void fminus(const XEvent& ev){
    GListBox * p = (GListBox*) GgetWindowMap(ID_OUTLIST);
    p->clear();
    fminusimpl();
};
void fminusimpl(){
    GListBox * p = (GListBox*) GgetWindowMap(ID_OUTLIST);
    for (int i = 0;i < 5; i++){
        p->addList(makeMinus());
    }
    p->update();
}
void fmultiply(const XEvent& ev){
    GListBox * p = (GListBox*) GgetWindowMap(ID_OUTLIST);
    p->clear();
    fmultiplyimpl();
};
void fmultiplyimpl(){
    GListBox * p = (GListBox*) GgetWindowMap(ID_OUTLIST);
    for (int i = 0;i < 5; i++){
        p->addList(makeMultiply());
    }
    p->update();
}
void ftest(const XEvent& ev){
    GListBox * p = (GListBox*) GgetWindowMap(ID_OUTLIST);
    p->clear();
   fplusimpl();
   fminusimpl();
   fmultiplyimpl();
};

int main(){
    //GFrameLayout manager("manager");
    GFrameLayout downleft("buttons"),
                 downright("results"),
                 down("operator");
    GM root("root window");

    GButton btnPlus("Plus"),
            btnMinus("Minus"),
            btnMultply("Multiply"),
            btnTest("Test");
            
    btnPlus.addEventHandler(fplus);
    btnMinus.addEventHandler(fminus);
    btnMultply.addEventHandler(fmultiply);
    btnTest.addEventHandler(ftest);

    GListBox outlist;
    outlist.addList("you see!");
    outlist.addMap(ID_OUTLIST);
    outlist.setMaxLine(15);
    downright.add(&outlist);


    downleft.add(&btnPlus);
    downleft.add(&btnMinus);
    downleft.add(&btnMultply);
    downleft.add(&btnTest);


    down.add(&downleft);
    down.add(&downright);
    down.addLayout(new GLayoutHori());
    down.addMap(ID_FRAMEDOWN);

    //manager.add(&down);
    //root.add(&manager);
    root.addFrame(&down);

    srandom(time(NULL));

    root.run();
    return 0;
}
