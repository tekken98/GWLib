#include <iostream>
#include <vector>
#include <exception>
#include <memory>
#include "GWindow.h"
//#include <typeinfo>
#define ID_LIST 1001
class dlg : public GWindow<dlg>
{
    GFrameLayout*  manager = NULL;
    GButton * ok = NULL;
    GButton * cancel = NULL;
    public:
    dlg(){
        manager = (new GFrameLayout("dlg"));
        ok = new GButton("ok");
        cancel = new GButton("cancel");
        manager->add(ok);
        manager->add(cancel);
        manager->addLayout(new GLayoutHori());
        addFrame(manager);
        setWindowName("Dlg");
        setWindowRectInParent(0,0,200,200);
    }
    void show(){
        run();
    }
    ~dlg(){
        delete ok;
        delete cancel;
        delete manager;
    }
    void draw(){};
};
class MW : public GWindow<MW>
{
    GList * m_list;
    public:
    void draw(){
        msg(m_list->getTitle());
    }
    MW(){
        //setWindowRectInParent(0,0,400,400);
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
        char ** fonts;
        int font_count;
        fonts = XListFonts(getDisplay(),"*gb*",10000,&font_count);
        for (int i = 0; i< font_count; i++)
        {
            lb.addList(fonts[i]);
        }
        XFreeFontNames(fonts);


        mid.add(&lb);
        //mid.add(&l);
        //mid.add(&bb);
        //mid.add(&label);
        //mid.add(&lll);
       // mid.add(&lb);
       // mid.add(&btn);
       low.add(&left);
       low.add(&right);

       GButton mb("okman");
       mb.setFont();
       right.add(&mb);
        manager.addLayout(new GLayoutHori());
        up.addLayout(new GLayoutVertical());
        mid.addLayout(new GLayoutHori());
        right.addLayout(new GLayoutVertical());
        left.addLayout(new GLayoutVertical());
        low.addLayout(new GLayoutHori());
        innerup.addLayout(new GLayoutVertical());
        innerdown.addLayout(new GLayoutVertical());
        left.add(&innerup);
        left.add(&innerdown);
        
        //manager.add(&up);
        manager.add(&mid);
        manager.add(&low);
        addFrame(&manager);
        manager.setBorderWidth(0);
        setWindowName("fun");
        run();
    }
};
int main () {
    //dlg d;
    //d.show();
    MW w;
    return 0;
}
