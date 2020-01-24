#include <iostream>
#include <vector>
#include <exception>
#include "GWindow.h"
//#include <typeinfo>
#define ID_LIST 1001
class MW : public GWindow<MW>
{
    GList * m_list;
    public:
    void draw(){
        msg(m_list->getTitle());
    }
    MW(){
        GFrameLayout manager("Manager");
        GFrameLayout up("up frame"), 
                     mid("mid frame"), 
                     right("right frame"),
                     low("low frame"),
                     innerup("inner frame"),
                     innerdown("inner frame"),
                     left("left frame");
        GList list;
        GEdit ed("one"), ed2("two");

        list.addList("one");
        list.addList("two");
        GList apple;
        apple.addList("big");
        apple.addList("small");
        apple.addList("mid");
        m_list = &apple;
        up.add(&list);
        up.add(&apple);
        up.add(&ed);
        up.add(&ed2);
        GList l;
        GEdit label("good");
        l.addList("good");
        l.addList("bad");
        GListBox lb;
        GListBox bb;
        GString str[] {"one","two","three","four","five","six","seven"};
        for (int i = 0;i < 7;i++){
            l.addList(str[i]);
            bb.addList(str[i]);
            list.addList(str[i]);
        }
        GLabel lll("good");
        GLabel btn("button");

        lb.addList("good");
        lb.addList("bad1");
        lb.addList("bad2");
        lb.addList("bad3");
        lb.addList("bad4");
        lb.addList("bad5");
        lb.addList("bad6");
        lb.addList("bad7");
        lb.addList("bad8");
        lb.addList("bad9");
        lb.addList("no");


        mid.add(&lb);
        mid.add(&l);
        mid.add(&bb);
        mid.add(&label);
        //mid.add(&lll);
       // mid.add(&lb);
       // mid.add(&btn);
       low.add(&left);
       low.add(&right);

       GButton mb("ok");
       right.add(&mb);

        manager.addLayout(new GLayoutVertical());
        up.addLayout(new GLayoutVertical());
        mid.addLayout(new GLayoutHori());
        right.addLayout(new GLayoutVertical());
        left.addLayout(new GLayoutVertical());
        low.addLayout(new GLayoutHori());
        innerup.addLayout(new GLayoutVertical());
        innerdown.addLayout(new GLayoutVertical());
        left.add(&innerup);
        left.add(&innerdown);
        
        manager.add(&up);
        manager.add(&mid);
        //manager.add(&down);
        manager.add(&low);
        addFrame(&manager);
        manager.setBorderWidth(0);
        setWindowName("fun");
        run();

    }
};
class myexception : public exception
{
    public:
        void msg() {
            cout << "here " << endl;
        }
};
void foo() {
    int a = 0;
    if (a == 0){
            throw(new myexception());
    }  
    return;
}
int main () {
    //MW w;
    try{
        foo();
    }catch (myexception * p){
        p->msg();
    }
    return 0;
}
