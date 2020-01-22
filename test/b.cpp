#include "../GWindow.h"
class GM : public GWindow<GM>
{
    public:
    GM(const GString& s){
        setTitle(s);
    }
    void draw(){};
};
GListBox outlist;

int limit(int a, int b){
    unsigned long r = random();
    int ret;
    ret = r % (b - a + 1) + a;
    return ret;
}
GString makePlus(){
    GString a;
    int first = limit(1,20) ;
    int second = limit(1,20);
    while( first + second > 20){
        first = limit(1,20) ;
        second = limit(1,20);
    }
    a.format("%2d + %2d = %2d" , first, second , first + second);
    return a;
}
GString makeMinus(){
    GString a;
    int first = limit(1,20) ;
    int second = limit(1,20);
    int tmp;
    while( first == second){
        first = limit(1,20);
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

void fplus(const XEvent& ev){
    outlist.clear();
    for (int i = 0;i < 5; i++){
        outlist.addList(makePlus());
    }
    outlist.update();
};
void fminus(const XEvent& ev){
    outlist.clear();
    for (int i = 0;i < 5; i++){
        outlist.addList(makeMinus());
    }
    outlist.update();
};
void fmultiply(const XEvent& ev){
    outlist.clear();
    for (int i = 0;i < 5; i++){
        outlist.addList(makeMultiply());
    }
    outlist.update();
};

int main(){
    GFrameLayout manager("manager");
    GFrameLayout downleft("buttons"),
                 downright("results"),
                 down("operator");
    GM root("root window");

    GButton btnPlus("Plus"),
            btnMinus("Minus"),
            btnMultply("Multiply");
    btnPlus.addEventHandler(fplus);
    btnMinus.addEventHandler(fminus);
    btnMultply.addEventHandler(fmultiply);
    
    outlist.addList("you see!");
    outlist.setMaxline(10);

    downleft.add(&btnPlus);
    downleft.add(&btnMinus);
    downleft.add(&btnMultply);

    downright.add(&outlist);

    down.add(&downleft);
    down.add(&downright);
    down.addLayout(new GLayoutHori());

    manager.add(&down);
    root.add(&manager);
    root.setWindowName("Homework");
    root.run();
    return 0;
}
