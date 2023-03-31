#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <math.h>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <fstream>

using namespace sf;
using namespace std;

float animSpeed = 0.f;
bool animViz = 1;
int fileNumber;

FloatRect fendProcessButton(Vector2f(633, 698.123), Vector2f(95.938, 30.483));

struct Map {
    int map[30][50] = { 0 };
    RectangleShape square[30][50];
    vector<int> graph[1500];
}mapi;

class GameState {
public:
    enum States {
        MainMenu,
        InGame,
        ControlMenu,
        LoadingMenu
    };
    States state;
};

class Tool {
public:
    enum ToolType {
        Wall,
        Start,
        Finish,
        Erase,

        Count
    };
    ToolType type;
};

Color palette[6];

class LoadingMap {
public:
    Sprite shape;
    int map[30][50] = { 0 };
    string name;
    Text nameText;

    void SetupText(Font font) {
        nameText.setString(name);
        nameText.setPosition(shape.getPosition() + Vector2f(15, 3));
        //nameText.setFont(font);
        nameText.setCharacterSize(40);
        nameText.setStyle(Text::Bold);
    }

    LoadingMap(Sprite sp, string n, Font f, int x, int y) {
        shape = sp;
        name = n;
        shape.setPosition(x, y);
        SetupText(f);
    }
};

void SetupPalette() {
    palette[0] = Color::White;
    palette[1] = Color::Black;
    palette[2] = Color(233, 138, 46);   //Orange
    palette[3] = Color(213, 82, 90);    //Red
    palette[4] = Color(102, 161, 79);   //Green
    palette[5] = Color(233, 199, 76);   //Yellow
}

void SetupGraph() {
    for (int i = 0; i < 30; i++) {
        for (int j = 0; j < 50; j++) {
            int primul = 50 * i + j;
            if (i > 0) {
                int doilea = primul - 50;
                mapi.graph[primul].push_back(doilea);
            }
            if (j < 49) {
                int doilea = primul + 1;
                mapi.graph[primul].push_back(doilea);
            }
            if (i < 29) {
                int doilea = primul + 50;
                mapi.graph[primul].push_back(doilea);
            }
            if (j > 0) {
                int doilea = primul - 1;
                mapi.graph[primul].push_back(doilea);
            }

        }
    }
}

void SetupMapSquares() {
    Vector2f siz = Vector2f(20, 20);
    for (int i = 0; i <= 29; i++) {
        for (int j = 0; j <= 49; j++) {

            Vector2f pos = Vector2f(j * 20, i * 20);

            mapi.square[i][j].setPosition(pos);
            mapi.square[i][j].setSize(siz);
            mapi.square[i][j].setFillColor(Color::White);

        }
    }
}

void RenderGraph() {
    for (int i = 0; i <= 29; i++) {
        for (int j = 0; j <= 49; j++) {

            int currentCol = mapi.map[i][j];

            mapi.square[i][j].setFillColor(palette[currentCol]);

        }
    }
}

bool isUnVisited(vector<int>q, int x) {
    for (auto i : q) {
        if (i == x)
            return 0;
    }
    int xx = x / 50;
    int y = x - xx * 50;
    if (mapi.map[xx][y] == 1)
        return 0;
    return 1;
}

void ResetMap() {
    for (int i = 0; i < 30; i++) {
        for (int j = 0; j < 50; j++) {
            mapi.map[i][j] = 0;
        }
    }
}

vector<int> BFSShortestPath(int a, int b, RenderWindow& window) {
    vector<int>q;
    int cur = a;
    q.push_back(a);
    int j = 0;
    int inherit[1500] = { 0 };
    bool sem = 1;

    do {
        for (int i = 0; i < mapi.graph[cur].size(); i++) {
            if (isUnVisited(q, mapi.graph[cur][i])) {
                if (mapi.graph[cur][i] == b)
                    sem = 0;
                int x = mapi.graph[cur][i] / 50;
                int y = mapi.graph[cur][i] % 50;
                mapi.map[x][y] = 5;

                mapi.square[x][y].setFillColor(Color(233, 199, 76));

                Event event;
                while (window.pollEvent(event)) {
                    if (event.type == Event::Closed)
                        window.close();
                    if (event.type == Event::MouseButtonPressed) {
                        Vector2f point = Vector2f(Mouse::getPosition(window).x, Mouse::getPosition(window).y);
                        if (fendProcessButton.contains(point)) {
                            vector<int>empty;
                            return empty;
                        }
                    }
                }

                if (animViz) {
                    window.draw(mapi.square[x][y]);

                    window.display();

                    window.draw(mapi.square[x][y]);
                }

                sleep(seconds(animSpeed));

                q.push_back(mapi.graph[cur][i]);

                inherit[mapi.graph[cur][i]] = cur;
            }
        }
        if (q.size() == j + 1) {
            vector<int>empty;
            return empty;
        }

        j++;
        cur = q[j];

    } while (sem);

    int i = b;
    vector<int>sol;
    while (i != a) {
        sol.push_back(inherit[i]);
        i = inherit[i];
    }
    sol.push_back(b);
    return sol;
}

void DFS(int a, int b, RenderWindow& window, bool& isFound, vector<int>& path, int maxDepth, int currDepth) { ///a is the current node, b is the end
    if (a != b && !isFound) {
        if (currDepth <= maxDepth) {
            for (auto i : mapi.graph[a]) {
                if (!isFound && isUnVisited(path, i)) {
                    path.push_back(i);


                    Event event;
                    while (window.pollEvent(event)) {
                        if (event.type == Event::Closed)
                            window.close();
                        if (event.type == Event::MouseButtonPressed) {
                            Vector2f point = Vector2f(Mouse::getPosition(window).x, Mouse::getPosition(window).y);
                            if (fendProcessButton.contains(point))
                                isFound = 1;
                        }
                    }

                    if (animViz) {
                        mapi.square[i / 50][i % 50].setFillColor(Color(233, 199, 76));
                        window.draw(mapi.square[i / 50][i % 50]);
                        window.display();
                        window.draw(mapi.square[i / 50][i % 50]);
                    }

                    sleep(seconds(animSpeed));

                    DFS(i, b, window, isFound, path, maxDepth, currDepth + 1);

                }
            }
        }

    }
    else isFound = 1;
    if (!isFound && currDepth > 0) {
        path.pop_back();
        if (animViz) {
            mapi.square[a / 50][a % 50].setFillColor(Color::White);
            window.draw(mapi.square[a / 50][a % 50]);
            window.display();
            window.draw(mapi.square[a / 50][a % 50]);
        }
    }
}

vector<int> DFSShortestPath(int a, int b, RenderWindow& window) {
    vector<int>path;
    int maxDepth = 0;
    bool isFound = 0;
    while (!isFound) {
        path.clear();
        path.push_back(a);
        DFS(a, b, window, isFound, path, maxDepth, 0);
        maxDepth++;
    }
    path.pop_back();
    path.erase(path.begin());
    path.push_back(a);
    path.push_back(b);

    return path;
}

bool DrawShortestPath(vector<int>q) {
    int size = q.size() - 1;

    if (size >= 0) {
        mapi.map[q[size] / 50][q[size] - (q[size] / 50) * 50] = 3;
        size--;
        mapi.map[q[size] / 50][q[size] - (q[size] / 50) * 50] = 4;

        for (int i = 0; i < size; i++) {
            int x = q[i] / 50;
            int y = q[i] - x * 50;
            mapi.map[x][y] = 2;
        }
        return 0;
    }
    return 1;
}

void ClickOnGraph(Vector2f point, int col, int& node) {
    bool sem = 1;
    for (int i = 0; i <= 29 && sem; i++) {
        for (int j = 0; j <= 49 && sem; j++) {

            FloatRect sq = mapi.square[i][j].getGlobalBounds();
            node = i * 50 + j;

            if (sq.contains(point) && (!mapi.map[i][j] || !col)) {
                mapi.map[i][j] = col;
                sem = 0;
            }

        }
    }
}

void ResetColOnMap(Map& mapi, int col) {
    for (int i = 0; i < 30; i++) {
        for (int j = 0; j < 50; j++) {
            if (mapi.map[i][j] == col)
                mapi.map[i][j] = 0;
        }
    }
}

void ResetPath(Map& mapi) {
    ResetColOnMap(mapi, 2);
    ResetColOnMap(mapi, 5);
}

void MenuDesignSetup(VertexArray& menuBg, RectangleShape menuFrame[4]) {
    ///Menu Bg
    Color bgCol = Color(185, 176, 171);

    menuBg[0].position = Vector2f(0, 600);
    menuBg[0].color = bgCol;
    menuBg[1].position = Vector2f(1000, 600);
    menuBg[1].color = bgCol;
    menuBg[2].position = Vector2f(1000, 800);
    menuBg[2].color = bgCol;
    menuBg[3].position = Vector2f(0, 800);
    menuBg[3].color = bgCol;

    ///Menu Frame
    Color menuFrameCol = Color(255, 255, 255);

    menuFrame[0].setSize(Vector2f(960, 10));
    menuFrame[1].setSize(Vector2f(10, 160));
    menuFrame[2].setSize(Vector2f(10, 160));
    menuFrame[3].setSize(Vector2f(960, 10));

    menuFrame[0].setPosition(Vector2f(20, 620));
    menuFrame[1].setPosition(Vector2f(970, 620));
    menuFrame[2].setPosition(Vector2f(20, 620));
    menuFrame[3].setPosition(Vector2f(20, 770));

    menuFrame[0].setFillColor(menuFrameCol);
    menuFrame[1].setFillColor(menuFrameCol);
    menuFrame[2].setFillColor(menuFrameCol);
    menuFrame[3].setFillColor(menuFrameCol);
}

void ButtonsSetup(VertexArray& buttons, Color palette[]) {
    int size = 100;
    int y = 650;

    buttons[0].color = palette[4];
    buttons[1].color = palette[4];
    buttons[2].color = palette[4];
    buttons[3].color = palette[4];

    buttons[4].color = palette[3];
    buttons[5].color = palette[3];
    buttons[6].color = palette[3];
    buttons[7].color = palette[3];

    buttons[8].color = palette[1];
    buttons[9].color = palette[1];
    buttons[10].color = palette[1];
    buttons[11].color = palette[1];

    int i = 0;

    for (int X = 60; X <= 510; X += 150) {
        buttons[i++].position = Vector2f(X, y);
        buttons[i++].position = Vector2f(X + size, y);
        buttons[i++].position = Vector2f(X + size, y + size);
        buttons[i++].position = Vector2f(X, y + size);
    }

}

void SetupButtonFrame(VertexArray& frame) {
    int x = 50, y = 640, sizex = 120, sizey = 5;

    frame[0].position = Vector2f(x, y);
    frame[1].position = Vector2f(x + sizex, y);
    frame[2].position = Vector2f(x + sizex, y + sizey);
    frame[3].position = Vector2f(x, y + sizey);

    y += 115;
    frame[4].position = Vector2f(x, y);
    frame[5].position = Vector2f(x + sizex, y);
    frame[6].position = Vector2f(x + sizex, y + sizey);
    frame[7].position = Vector2f(x, y + sizey);

    sizex = 5;
    sizey = 120;
    y -= 115;
    frame[8].position = Vector2f(x, y);
    frame[9].position = Vector2f(x + sizex, y);
    frame[10].position = Vector2f(x + sizex, y + sizey);
    frame[11].position = Vector2f(x, y + sizey);

    x += 115;
    frame[12].position = Vector2f(x, y);
    frame[13].position = Vector2f(x + sizex, y);
    frame[14].position = Vector2f(x + sizex, y + sizey);
    frame[15].position = Vector2f(x, y + sizey);

    for (int i = 0; i < 16; i++)
        frame[i].color = Color::White;
}

void MoveFrame(int caz, VertexArray& frame) {
    int  sizex = 120, sizey = 5;

    int x = (caz - 1) * 150 + 50, y = 640;

    frame[0].position = Vector2f(x, y);
    frame[1].position = Vector2f(x + sizex, y);
    frame[2].position = Vector2f(x + sizex, y + sizey);
    frame[3].position = Vector2f(x, y + sizey);

    y += 115;
    frame[4].position = Vector2f(x, y);
    frame[5].position = Vector2f(x + sizex, y);
    frame[6].position = Vector2f(x + sizex, y + sizey);
    frame[7].position = Vector2f(x, y + sizey);

    sizex = 5;
    sizey = 120;
    y -= 115;
    frame[8].position = Vector2f(x, y);
    frame[9].position = Vector2f(x + sizex, y);
    frame[10].position = Vector2f(x + sizex, y + sizey);
    frame[11].position = Vector2f(x, y + sizey);

    x += 115;
    frame[12].position = Vector2f(x, y);
    frame[13].position = Vector2f(x + sizex, y);
    frame[14].position = Vector2f(x + sizex, y + sizey);
    frame[15].position = Vector2f(x, y + sizey);
}

void SetVisibilityOfSprite(bool vizible, Sprite& sprite) {
    if (vizible) {
        sprite.setColor(Color::White);
    }
    else
        sprite.setColor(Color(255, 255, 255, 0));
}

void MapiMapLoadingSetup(int loadingMap[][50]) {
    for (int i = 0; i < 30; i++) {
        for (int j = 0; j < 50; j++) {
            mapi.map[i][j] = loadingMap[i][j];
        }
    }
}

void ReadAllMaps(vector<LoadingMap>& l, Sprite& loadButton, Font& f) {
    l.clear();
    ifstream fin("Maps.txt");
    int x = 30, y = 185;
    LoadingMap lm(loadButton, "", f, x, y);
    lm.nameText.setFont(f);
    int cont = 0;

    fileNumber = 0;
    while (getline(fin,lm.name)) {
        
        if (lm.name != "") {
            fileNumber++;
            for (int i = 0; i < 30; i++) {
                for (int j = 0; j < 50; j++) {
                    fin >> lm.map[i][j];
                }
            }
            cont++;

            lm.shape.setPosition(x, y);
            lm.nameText.setString(lm.name);
            lm.nameText.setPosition(x + 15, y + 3);
            l.push_back(lm);
            if (cont == 7) {
                x = 500;
                y = 110;
            }
            y += 75;
            fin.ignore();
        }
        
    }
    fin.close();
}

void DeleteMap(size_t selected) {
    ifstream fin("Maps.txt");
    vector<vector<string>>table;
    vector<string>chair;
    string curr;
    size_t lin = -1;
    while (getline(fin, curr)) {
        if (isalpha(curr[0])) {
            lin++;
            table.push_back(chair);
            chair.clear();
        }
        if (lin != selected) {
            chair.push_back(curr);
        }
    }
    table.push_back(chair);
    fin.close();

    ofstream fout("Maps.txt");
    for (const auto& i : table) {
        for (const auto& j : i) {
            fout << j << "\n";
        }
    }
    fout.close();
}

void AddMap(string name) {
    fileNumber++;
    ofstream fout;
    fout.open("Maps.txt", ios::app);
    fout <<  name << "\n";
    for (int i = 0; i < 30; i++) {
        for (int j = 0; j < 50; j++) {
            fout << mapi.map[i][j] << " ";
        }
        fout << "\n";
    }
    fout.close();
}

void GetStartEndPoint(int& s, int& e) {
    for (int i = 0; i < 30; i++) {
        for (int j = 0; j < 50; j++) {
            if (mapi.map[i][j] == 4) {   ///starting point
                s = i * 50 + j;
            }
            if (mapi.map[i][j] == 3) {   ///end point
                e = i * 50 + j;
            }
        }
    }
}

int main() {
    ///Game Setup
    RenderWindow window(VideoMode(1000, 800), "Proiect Grafuri");
    Tool tool;
    tool.type = Tool::Start;
    SetupPalette();
    SetupMapSquares();
    SetupGraph();
    GameState gameState;
    gameState.state = GameState::MainMenu;
    vector<FloatRect>allButtons;

    ///Menu Setup
    VertexArray menuBg(Quads, 4);
    RectangleShape menuFrame[4];
    MenuDesignSetup(menuBg, menuFrame);
    VertexArray buttons(Quads, 16);
    ButtonsSetup(buttons, palette);
    VertexArray chosenButtonFrame(Quads, 16);
    SetupButtonFrame(chosenButtonFrame);
    Texture GameButtonsTT;
    GameButtonsTT.loadFromFile("GameButtons.png");
    Sprite gameButtons;
    gameButtons.setTexture(GameButtonsTT);
    gameButtons.setPosition(Vector2f(631, 639.793));
    Font SansSerif;
    SansSerif.loadFromFile("OpenSans.ttf");
    Text speedText;
    speedText.setFont(SansSerif);
    speedText.setStyle(Text::Bold);
    speedText.setPosition(Vector2f(877, 694));

    ///Buttons Setup                POSITION,SIZE
    FloatRect fstartButton(Vector2f(60, 650), Vector2f(100, 100));
    FloatRect fendButton(Vector2f(210, 650), Vector2f(100, 100));
    FloatRect fwallButton(Vector2f(360, 650), Vector2f(100, 100));
    FloatRect feraseButton(Vector2f(510, 650), Vector2f(100, 100));
    FloatRect fstartProcessButton(Vector2f(633, 665.281), Vector2f(95.938, 30.483));

    FloatRect fbfsSwitchButton(Vector2f(839, 639.793), Vector2f(126.126, 28.924));
    FloatRect flowerSpeedButton(Vector2f(844.062, 703.653), Vector2f(32.796, 20.154));
    FloatRect fincreaseSpeedButton(Vector2f(926.373, 703.653), Vector2f(32.796, 20.154));
    FloatRect freturnToMenuButton(Vector2f(838, 735.683), Vector2f(126.128, 28.924));
    FloatRect saveMapButton(735.971, 665.281, 94.365, 29.983);
    FloatRect setAnimVizButton(735.971, 698.123, 94.365, 29.983);

    allButtons.push_back(fstartButton);
    allButtons.push_back(fendButton);
    allButtons.push_back(fwallButton);
    allButtons.push_back(feraseButton);
    allButtons.push_back(fstartProcessButton);
    allButtons.push_back(fbfsSwitchButton);
    allButtons.push_back(flowerSpeedButton);
    allButtons.push_back(fincreaseSpeedButton);
    allButtons.push_back(freturnToMenuButton);
    allButtons.push_back(saveMapButton);
    allButtons.push_back(setAnimVizButton);

    Texture DFSButtonTT;
    DFSButtonTT.loadFromFile("DFSButton.png");
    Sprite DFSButtonSprite;
    DFSButtonSprite.setTexture(DFSButtonTT);
    DFSButtonSprite.setPosition(Vector2f(839, 639.793));

    Texture vizOnTT;
    Texture vizOffTT;
    vizOnTT.loadFromFile("VizOn.png");
    vizOffTT.loadFromFile("VizOff.png");
    Sprite vizOn;
    Sprite vizOff;
    vizOn.setTexture(vizOnTT);
    vizOff.setTexture(vizOffTT);
    vizOn.setPosition(788.143, 705.223);
    vizOff.setPosition(784.431,705.223);

    ///Error Box setup
    Texture errorBoxTexture;
    errorBoxTexture.loadFromFile("ErrorBox.png");
    Sprite errorBox;
    errorBox.setTexture(errorBoxTexture);
    errorBox.setPosition(Vector2f(300, 200));

    ///MainMenu setup
    Texture MainMenuTT;
    MainMenuTT.loadFromFile("MainMenu.png");
    Sprite mainMenu;
    mainMenu.setTexture(MainMenuTT);
    RectangleShape startButton;
    startButton.setPosition(Vector2f(285, 410));
    startButton.setSize(Vector2f(435, 122));
    FloatRect startButtonHitbox = startButton.getGlobalBounds();
    RectangleShape controlsButton;
    controlsButton.setPosition(285, 568);
    controlsButton.setSize(Vector2f(435, 122));
    FloatRect controlsButtonHitbox = controlsButton.getGlobalBounds();

    ///Controls Menu Setup
    FloatRect controlReturnButtonHitbox(Vector2f(354, 699), Vector2f(312, 99));
    Texture ControlsMenuTT;
    ControlsMenuTT.loadFromFile("ControlsMenu.png");
    Sprite controlsMenu;
    controlsMenu.setTexture(ControlsMenuTT);

    ///Game Variables
    int StartNode = 0, EndNode = 1499;
    bool errorViz = 0;
    int bfsOrDfs = 1;
    bool isSPressed = 0;
    string playerInput;
    Text inputText;
    inputText.setFont(SansSerif);
    inputText.setFillColor(Color::Black);
    inputText.setStyle(Text::Bold);
    inputText.setPosition(225,310);

    ///LoadingMenuSetup
    Texture loadingMenuTT;
    loadingMenuTT.loadFromFile("LoadingMenu.png");
    Sprite LoadingMenu;
    LoadingMenu.setTexture(loadingMenuTT);
    Texture loadingButtonTT;
    loadingButtonTT.loadFromFile("LoadingButton.png");
    Sprite loadingButtonSprite;
    loadingButtonSprite.setTexture(loadingButtonTT);

    FloatRect loadButtonHit(Vector2f(90, 725), Vector2f(150, 55));
    FloatRect deleteButtonHit(Vector2f(275, 725), Vector2f(220, 55));

    Texture selectedFrameTT;
    selectedFrameTT.loadFromFile("SelectedFrame.png");
    Sprite selectedFrame;
    selectedFrame.setTexture(selectedFrameTT);
    selectedFrame.setPosition(20, 178);

    Texture newFileMenuTT;
    newFileMenuTT.loadFromFile("NewFileMenu.png");
    Sprite newFileMenu;
    newFileMenu.setTexture(newFileMenuTT);
    newFileMenu.setPosition(170,200);

    FloatRect okNewFileButtonHit(243.252, 385.317, 177.354, 56.327);
    FloatRect calncelNewFileButtonHit(547.017, 385.317, 177.354, 56.327);

    Texture loadingReturnButtonTT;
    loadingReturnButtonTT.loadFromFile("LoadReturnButton.png");
    Sprite loadReturnButton;
    loadReturnButton.setTexture(loadingReturnButtonTT);
    loadReturnButton.setPosition(701.656, 724.898);

    ///Loading Vector
    vector<LoadingMap>loadingMaps;
    size_t selected = 0;

    while (window.isOpen()) {
        Event event;
        switch (gameState.state) {
        case GameState::LoadingMenu: {
            ReadAllMaps(loadingMaps, loadingButtonSprite, SansSerif);
            while (window.pollEvent(event)) {
                if (event.type == Event::Closed)
                    window.close();
            }

            if (Mouse::isButtonPressed(Mouse::Left)) {
                Vector2i ipoint = Mouse::getPosition(window);
                Vector2f point = Vector2f(ipoint.x, ipoint.y);

                for (size_t i = 0; i < loadingMaps.size(); i++) {
                    if (loadingMaps[i].shape.getGlobalBounds().contains(point)) {
                        selected = i;
                        selectedFrame.setPosition(loadingMaps[i].shape.getPosition() + Vector2f(-10, -7));
                        break;
                    }
                }
                if (loadButtonHit.contains(point)) {
                    gameState.state = GameState::InGame;
                    MapiMapLoadingSetup(loadingMaps[selected].map);
                    GetStartEndPoint(StartNode, EndNode);
                }
                else if (selected && deleteButtonHit.contains(point)) {
                    DeleteMap(selected);
                    ReadAllMaps(loadingMaps, loadingButtonSprite, SansSerif);
                    selectedFrame.setPosition(loadingMaps[--selected].shape.getPosition() + Vector2f(-10, -7));
                    sleep(seconds(0.2));
                }
                else if (loadReturnButton.getGlobalBounds().contains(point)) {
                    gameState.state = GameState::MainMenu;
                }
            }

            window.clear();
            window.draw(LoadingMenu);
            for (const auto& i : loadingMaps) {
                window.draw(i.shape);
                window.draw(i.nameText);
            }
            window.draw(selectedFrame);
            window.draw(loadReturnButton);

            window.display();



            break;
        }
        case GameState::MainMenu: {
            while (window.pollEvent(event)) {
                if (event.type == Event::Closed)
                    window.close();
            }

            if (Mouse::isButtonPressed(Mouse::Left)) {
                Vector2i ipoint = Mouse::getPosition(window);
                Vector2f point = Vector2f(ipoint.x, ipoint.y);
                if (startButtonHitbox.contains(point)) {
                    gameState.state = GameState::LoadingMenu;
                }
                if (controlsButtonHitbox.contains(point)) {
                    gameState.state = GameState::ControlMenu;
                }
            }

            window.clear();

            window.draw(mainMenu);

            window.display();
            break;
        }
        case GameState::ControlMenu: {
            while (window.pollEvent(event)) {
                if (event.type == Event::Closed)
                    window.close();
                if (event.type == Event::MouseButtonPressed) {
                    Vector2i ipoint = Mouse::getPosition(window);
                    Vector2f point = Vector2f(ipoint.x, ipoint.y);
                    if (controlReturnButtonHitbox.contains(point))
                        gameState.state = GameState::MainMenu;
                }
            }
            window.clear();
            window.draw(controlsMenu);
            window.display();
            break;
        }
        case GameState::InGame: {
            ///Events
            while (window.pollEvent(event)) {
                if (event.type == Event::Closed) {
                    window.close();
                }

                if (isSPressed) {
                    if (event.type == Event::TextEntered){
                        if (event.text.unicode != 8 && inputText.getGlobalBounds().width<520) {
                            playerInput += event.text.unicode;
                            inputText.setString(playerInput);
                        }
                        else if(playerInput.size()){
                            playerInput.pop_back();
                            inputText.setString(playerInput);
                        }
                    }
                }
            }

            if (Mouse::isButtonPressed(Mouse::Left) && isSPressed) {
                Vector2f point = Vector2f(Mouse::getPosition(window).x, Mouse::getPosition(window).y);
                if (okNewFileButtonHit.contains(point) && playerInput!="" && fileNumber<14) {
                    AddMap(playerInput);
                    isSPressed = 0;
                    playerInput = "";
                    inputText.setString(playerInput);
                    sleep(seconds(0.2));
                }
                if (calncelNewFileButtonHit.contains(point)) {
                    isSPressed = 0;
                    playerInput = "";
                    inputText.setString(playerInput);
                    sleep(seconds(0.2));
                }
            }

            if (Mouse::isButtonPressed(Mouse::Left)) {
                if (errorViz == 1) {
                    errorViz = 0;
                }

                Vector2f point = Vector2f(Mouse::getPosition(window).x, Mouse::getPosition(window).y);

                ResetColOnMap(mapi, 2);
                ResetColOnMap(mapi, 5);

                FloatRect* but = &fendProcessButton;

                for (auto& i : allButtons) {
                    if (i.contains(point)) {
                        but = &i;
                        break;
                    }
                }

                if (*but == fstartButton) {
                    tool.type = Tool::Start;
                    MoveFrame(1, chosenButtonFrame);
                }
                else if (*but == fendButton) {
                    tool.type = Tool::Finish;
                    MoveFrame(2, chosenButtonFrame);
                }
                else if (*but == fwallButton) {
                    tool.type = Tool::Wall;
                    MoveFrame(3, chosenButtonFrame);
                }
                else if (*but == feraseButton) {
                    tool.type = Tool::Erase;
                    MoveFrame(4, chosenButtonFrame);
                }
                else if (*but == fstartProcessButton) {
                    ResetPath(mapi);
                    if (bfsOrDfs == 1)
                        errorViz = DrawShortestPath(BFSShortestPath(StartNode, EndNode, window));
                    else
                        errorViz = DrawShortestPath(DFSShortestPath(StartNode, EndNode, window));
                }
                else if (*but == fbfsSwitchButton) {
                    bfsOrDfs *= -1;
                    sleep(seconds(0.1));
                }
                else if (*but == flowerSpeedButton) {
                    if (animSpeed > 0.1f)animSpeed -= 0.1;
                    sleep(seconds(0.1));
                }
                else if (*but == fincreaseSpeedButton) {
                    if (animSpeed < 9.8)animSpeed += 0.1;
                    sleep(seconds(0.1));
                }
                else if (*but == freturnToMenuButton) {
                    gameState.state = GameState::MainMenu;
                }
                else if (*but == saveMapButton) {
                    isSPressed = 1;
                }
                else if (*but == setAnimVizButton) {
                    animViz = !animViz;
                    sleep(seconds(0.2));
                }
                else if (point.y <= 600)
                    switch (tool.type) {
                    case Tool::Wall:
                        int empty;
                        ClickOnGraph(point, 1, empty);
                        break;

                    case Tool::Start:
                        ResetColOnMap(mapi, 4);
                        ClickOnGraph(point, 4, StartNode);
                        break;

                    case Tool::Finish:
                        ResetColOnMap(mapi, 3);
                        ClickOnGraph(point, 3, EndNode);
                        break;
                    case Tool::Erase:
                        int emptY;
                        ClickOnGraph(point, 0, emptY);
                        break;
                    }
            }

            ///Render Graph
            RenderGraph();

            stringstream stream;
            stream << fixed << setprecision(1) << 9.9f -animSpeed;
            string speedFloatToString = stream.str();
            speedText.setString(speedFloatToString);

            ///Draw
            window.clear();

            window.draw(menuBg);
            for (int i = 0; i <= 3; i++)
                window.draw(menuFrame[i]);
            for (int i = 0; i < 30; i++) {
                for (int j = 0; j < 50; j++)
                    window.draw(mapi.square[i][j]);
            }
            window.draw(buttons);
            window.draw(chosenButtonFrame);
            if (errorViz)
                window.draw(errorBox);
            window.draw(gameButtons);
            if (bfsOrDfs == -1)
                window.draw(DFSButtonSprite);
            window.draw(speedText);
            if (isSPressed) {
                window.draw(newFileMenu);
                window.draw(inputText);
            }
            if (animViz)
                window.draw(vizOn);
            else
                window.draw(vizOff);

            window.display();
            break;
        }
        }
    }
}





