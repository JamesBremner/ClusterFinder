#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include "cxy.h"
#include <wex.h>
#include "cRunWatch.h"

std::vector<cxy> vTarget;
double theDim;
int theMax2;
cxy bestAim;

void generate(double dim, double count)
{
    theDim = dim;

    std::vector<cxy> vCenter;
    for (int ck = 0; ck < count; ck++)
    {
        int x = rand() % (int)dim;
        int y = rand() % (int)dim;
        vCenter.emplace_back(x, y);
    }

    for (int tk = 0; tk < count * 10; tk++)
    {
        int c = rand() % (int)count;
        int x = vCenter[c].x + rand() % (int)dim / 10;
        int y = vCenter[c].y + rand() % (int)dim / 10;
        vTarget.emplace_back(x, y);
    }
    std::cout << "generated " << vTarget.size() << " targets\n";
}

void findBiggestCluster(double radius)
{
     raven::set::cRunWatch aWatcher("findBiggestCluster");
    theMax2 = radius * radius;
    int bestCount = 0;
    for (double y = 1; y < theDim; y++)
        for (double x = 1; x < theDim; x++)
        {
            cxy aim((int)x, (int)y);
            int count = 0;
            for (cxy &t : vTarget)
            {
                if (aim.dist2(t) < theMax2)
                    count++;
            }
            if (count > bestCount)
            {
                bestCount = count;
                bestAim = aim;
            }
        }
}


class cGUI
{
public:
    cGUI()
        : fm(wex::maker::make())
    {


        raven::set::cRunWatch::Start();
        generate(60, 50);
        findBiggestCluster(5);
        raven::set::cRunWatch::Report();



        fm.move(50, 50, 500, 100);
        fm.text("ClusterFinder");

        fm.events().draw(
            [&](PAINTSTRUCT &ps)
            {
                wex::shapes S(ps);
                draw(S);
            });

        fm.show();
        fm.run();
    }

    virtual void draw(wex::shapes &S)
    {
        S.color(0x0000FF);
        S.fill(false);
        S.circle((int)bestAim.x*10,(int)bestAim.y*10,40);

        S.color(0xFF0000);
        //S.fill();
        for( cxy e : vTarget ) {
            e.x *= 10;
            e.y *= 10;
            S.rectangle( {
                (int)e.x-2,(int)e.y-2,
                5,5
            } );
        }

    }

protected:
    wex::gui &fm;
};

main()
{
    cGUI gui;

    return 0;
}
