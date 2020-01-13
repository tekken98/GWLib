#include <iostream>
#include <vector>
#include "GWindow.h"
class MW : public GWindow<MW>
{
    public:
        void draw(){
            msg("MW called");
            for (int i = 1; i < 20;i++){
                drawLine(0,i*20,400,i*20);
                GString s;
                s.format("%d.",i);
                drawString(0,i*20,s);
            }
        }
} w;
int main () {
    GFrameLayout manager("Manager");
    GFrameLayout up("up frame"), mid("mid frame"), down("down frame");
    GList list;
    GEdit ed("one"), ed2("two");

    list.addList("one");
    list.addList("two");
    GList apple;
    apple.addList("big");
    apple.addList("small");
    apple.addList("mid");
    up.add(&list);
    up.add(&apple);
    up.add(&ed);
    up.add(&ed2);
    
    GList l;
    GEdit label("good");
    l.addList("good");
    l.addList("bad");
    GString str[] {"one","two","three",
        "four","five"};
    for (int i = 0;i < 5;i++)
        l.addList(str[i]);
    GLabel lll("good");
    GLabel btn("button");
    mid.add(&l);
    mid.add(&label);
    mid.add(&lll);
    mid.add(&btn);


    manager.addLayout(new GLayoutVertical());
    up.addLayout(new GLayoutVertical());
    mid.addLayout(new GLayoutHori());
    down.addLayout(new GLayoutVertical());
    manager.add(&up);
    manager.add(&mid);
    manager.add(&down);
    w.addFrame(&manager);
    w.run();
    return 0;
}
