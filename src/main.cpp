#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <time.h>

#include "cxy.h"
#include "KMeans.h"
#include "quadtree.h"
#include <wex.h>
#include "cRunWatch.h"

std::vector<cxy> vTarget;
quad::cCell *theQuadtree;
double theDim;
int radius;
int theMax2;
cxy bestAim;
int bestCount;

void generate(double dim, double count)
{
    theDim = dim;

    srand(time(NULL));

    std::vector<cxy> vCenter;
    for (int ck = 0; ck < 5; ck++)
    {
        int x = rand() % (int)dim;
        int y = rand() % (int)dim;
        vCenter.emplace_back(x, y);
    }

    for (int tk = 0; tk < count * 10; tk++)
    {
        int c = rand() % 5;
        int x = vCenter[c].x + rand() % (int)dim / 10;
        int y = vCenter[c].y + rand() % (int)dim / 10;
        vTarget.emplace_back(x, y);
    }
    std::cout << "generated " << vTarget.size() << " targets\n";
}

void buildQuadtree()
{
    raven::set::cRunWatch aWatcher("buildQuadtree");
    theQuadtree = new quad::cCell(quad::cPoint(theDim / 2, theDim / 2), theDim);
    for (cxy &t : vTarget)
        theQuadtree->insert(quad::cPoint(t.x, t.y));
}

int countNeighbors(const cxy &aim)
{
    int count = 0;
    for (cxy &t : vTarget)
    {
        if (aim.dist2(t) < theMax2)
            count++;
    }
    return count;
}

void gridSearch(double radius)
{
    raven::set::cRunWatch aWatcher("gridSearch");
    theMax2 = radius * radius;
    bestCount = 0;
    for (double y = 1; y < theDim; y++)
        for (double x = 1; x < theDim; x++)
        {
            cxy aim((int)x, (int)y);
            int count = countNeighbors(aim);
            if (count > bestCount)
            {
                bestCount = count;
                bestAim = aim;
            }
        }
}

void quadSearch(double radius)
{
    raven::set::cRunWatch aWatcher("quadSearch");

    bestCount = 0;
    for (double y = 1; y < theDim; y++)
        for (double x = 1; x < theDim; x++)
        {
            cxy aim((int)x, (int)y);

            int count = theQuadtree->find(
                                       quad::cCell(quad::cPoint(aim.x, aim.y), 2 * radius))
                            .size();

            if (count > bestCount)
            {
                bestCount = count;
                bestAim = aim;
            }
        }
}

void findClustersKMeans(double radius)
{
    raven::set::cRunWatch aWatcher("findClustersKMeans");

    // Construct the KMeans class
    KMeans KM;

    theMax2 = radius * radius;

    // loop over data
    for (cxy &l : vTarget)
    {
        // construct data point
        cDataPoint dp(2);
        dp.d[0] = l.x;
        dp.d[1] = l.y;

        // add data point to KMeans
        KM.Add(dp);
    }

    // Initialize KMeans with required number of clusters
    KM.Init(4, false);

    // run KMeans algorithm to find clusters
    KM.Iter(20);

    bestCount = 0;
    for (auto &c : KM.clusters())
    {
        cxy aim;
        aim.x = c.center().d[0];
        aim.y = c.center().d[1];
        int count = countNeighbors(aim);
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
    cGUI();

    void draw(wex::shapes &S);

private:
    wex::gui &fm;

    int Gridhits;
    int QuadHits;
    int KmeansHits;
    cxy GridAim;
    cxy QuadAim;
    cxy KmeansAim;

    enum class eAlgo
    {
        grid,
        quad,
        kmeans,
    };

    eAlgo myAlgo;

    void search(eAlgo a);

    void constructMenu();
};

cGUI::cGUI()
    : fm(wex::maker::make()),
      myAlgo(eAlgo::grid)
{

    raven::set::cRunWatch::Start();
    radius = 40;
    generate(500, 500);
    search(eAlgo::grid);
    search(eAlgo::kmeans);
    search(eAlgo::quad);

    raven::set::cRunWatch::Report();

    fm.move(50, 50, 700, 700);
    fm.text("ClusterFinder");

    constructMenu();

    fm.events().draw(
        [&](PAINTSTRUCT &ps)
        {
            wex::shapes S(ps);
            draw(S);
        });

    fm.show();
    fm.run();
}

void cGUI::constructMenu()
{
    wex::menubar mbar(fm);

    wex::menu view(fm);
    view.append(
        "Grid",
        [&](const std::string &title)
        {
            myAlgo = eAlgo::grid;
            fm.text("ClusterFinder Grid search");
            fm.update();
        });
    view.append(
        "Quad",
        [this](const std::string &title)
        {
            myAlgo = eAlgo::quad;
            fm.text("ClusterFinder Quadtree search");
            fm.update();
        });
    view.append(
        "Kmeans",
        [this](const std::string &title)
        {
            myAlgo = eAlgo::kmeans;
            fm.text("ClusterFinder Kmeans search");
            fm.update();
        });
    mbar.append("View", view);
}
void cGUI::draw(wex::shapes &S)
{
    switch (myAlgo)
    {
    case eAlgo::grid:
        bestAim = GridAim;
        break;

    case eAlgo::quad:
        bestAim = QuadAim;
        break;

    case eAlgo::kmeans:
        bestAim = KmeansAim;
        break;
    }
    int scale = 1;
    S.color(0x0000FF);
    S.fill(false);
    S.circle((int)bestAim.x * scale, (int)bestAim.y * scale, radius);

    S.color(0xFF0000);
    // S.fill();
    for (cxy e : vTarget)
    {
        e.x *= scale;
        e.y *= scale;
        S.rectangle({(int)e.x - 2, (int)e.y - 2,
                     5, 5});
    }
}

void cGUI::search(eAlgo a)
{
    switch (a)
    {
    case eAlgo::grid:
        gridSearch(radius);
        std::cout << "Best count, grid " << bestCount << "\n";
        Gridhits = bestCount;
        GridAim = bestAim;
        break;

    case eAlgo::quad:
        buildQuadtree();
        quadSearch(radius);
        std::cout << "Best count, quadtree " << bestCount << "\n";
        QuadHits = bestCount;
        QuadAim = bestAim;
        break;

    case eAlgo::kmeans:
        findClustersKMeans(radius);
        std::cout << "Best count, KMeans " << bestCount << "\n";
        KmeansHits = bestCount;
        KmeansAim = bestAim;
        break;
    }
}

main()
{
    cGUI gui;

    return 0;
}
