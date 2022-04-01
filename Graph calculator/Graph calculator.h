#pragma once

#include "resource.h"
#include "includes.h"

struct Eqelem {
    short elemType = 0; // 0 - number  1 - constant symbol  2 - unary operation  3 - binary operations
    double data0=0;
    char data1 = '\0'; 
    string data2 = "";
    char data3 = '\0';
};

struct appState {
    short GraphType = 0; // 0 - euclidean plane  1 - complex plane          default - 0
    short EqType = 1; // 1<<0 - eq 1<<1 - L 1<<2 - R            default - 1
    pair<int, int> screenSize = { 400,400 };
    pair<double, double> topleft = { -2,2 };
    double perPixel = 1e-2;
    vector<Eqelem> Eq;
};

short L = 2, R = 4, E = 1; //EqType bitmask
 // >  <  =


POINT Pair2PT(pair<double, double> _in_, appState& StateInfo) {
    POINT ret;
    ret.x = (_in_.first - StateInfo.topleft.first)/StateInfo.perPixel;
    ret.y = (-_in_.second + StateInfo.topleft.second) / StateInfo.perPixel;
    return ret;
}

pair<double, double> PT2Pair(POINT _in_, appState& StateInfo) {
    pair<double, double> ret;
    ret.first = StateInfo.topleft.first + _in_.x * StateInfo.perPixel;
    ret.second = StateInfo.topleft.second - _in_.y * StateInfo.perPixel;
    return ret;
}

void incodeEq(string& _in_, appState& StateInfo);
vector<Eqelem> InFix2PostFix(vector<Eqelem>& _in_);
bool inGraph(pair<double, double> _in_, appState& StateInfo);
double calcEq0(double x, double y, vector<Eqelem>& Eq);
double calcEq1(double Re, double Im, vector<Eqelem>& Eq);
double norm(pair<double, double> x);
pair<double, double> grad(double x, double y,double delta, vector<Eqelem>& Eq);

void incodeEq(string& _in_, appState& StateInfo) {
    int now = 0;
    vector<Eqelem> temp[2];
    for (int i = 0; i < _in_.length(); i++) {
        auto item = _in_[i];
        if (item == '=' || item == '<' || item == '>') now = 1;
        if (item == '=') StateInfo.EqType |= E;
        else if (item == '<') StateInfo.EqType |= R;
        else if (item == '>') StateInfo.EqType |= L;
        else {
            Eqelem nowelem;
            if (item >= '0' && item <= '9') { 
                nowelem.elemType = 0;
                nowelem.data0 = item - '0';
                while (i < _in_.length()) {
                    if (_in_[i + 1] < '0' || _in_[i + 1]>'9') break;
                    i++;
                    nowelem.data0 *= 10;
                    nowelem.data0 += _in_[i] - '0';
                }
            }
            else if (item == '+' || item == '-' || item == '*' || item == '/' || item == '^') {
                nowelem.elemType = 3;
                nowelem.data3 = item;
            }
            else if (item == 'x' || item == 'y') {
                nowelem.elemType = 1;
                nowelem.data1 = item;
            }
            else {
                nowelem.elemType = 2;
                if (item == '(' || item == ')') nowelem.data2.push_back(item);
                else {
                    nowelem.data2.push_back(item);
                    while (i < _in_.length()) {
                        if (_in_[i + 1] =='(') break;
                        i++;
                        nowelem.data2.push_back(_in_[i]);
                    }
                }
            }
            temp[now].push_back(nowelem);
        }
    }
    Eqelem subs = { 3,0,'\0',"",'-' };
    vector<Eqelem> ltemp, rtemp;
    ltemp = InFix2PostFix(temp[0]);
    rtemp = InFix2PostFix(temp[1]);
    StateInfo.Eq = ltemp;
    StateInfo.Eq.insert(StateInfo.Eq.end(), rtemp.begin(), rtemp.end());
    StateInfo.Eq.push_back(subs);
    return;
}

vector<Eqelem> InFix2PostFix(vector<Eqelem>& _in_) {
    stack<Eqelem> opStack;
    vector<Eqelem> PostFix;
    for (auto item : _in_) {
        if (item.elemType == 0 || item.elemType == 1) {
            PostFix.push_back(item);
            if (opStack.empty()) continue;
            if (opStack.top().elemType == 3) {
                PostFix.push_back(opStack.top());
                opStack.pop();
            }
            
        }
        else if (item.elemType == 3) opStack.push(item);
        else if (item.elemType == 2) {
            if (item.data2 == ")") {
                if (opStack.top().data2 != "(") PostFix.push_back(opStack.top());
                opStack.pop();
                if (opStack.empty()) continue;
                if (opStack.top().elemType == 3) {
                    PostFix.push_back(opStack.top());
                    opStack.pop();
                }
            }
            else {
                opStack.push(item);
            }
        }
    }
    while (!opStack.empty()) {
        PostFix.push_back(opStack.top());
        opStack.pop();
    }
    return PostFix;
}

bool inGraph(pair<double, double> _in_, appState& StateInfo) { //|f-g|<epsilon*(f-g)'
    if ((StateInfo.EqType & (L | R)) == (L | R)) return false;
    bool ret = false;
    double Result, Grad = norm(grad(_in_.first, _in_.second, 1e-2, StateInfo.Eq));
    if (StateInfo.GraphType == 0) {
        Result = calcEq0(_in_.first, _in_.second, StateInfo.Eq);
    }
    else if (StateInfo.GraphType == 1) {
        Result = calcEq1(_in_.first, _in_.second, StateInfo.Eq);
    }
    if ((StateInfo.EqType & E) && abs(Result) < StateInfo.perPixel * Grad) ret = true;
    if ((StateInfo.EqType & L) && Result > 0) ret = true;
    if ((StateInfo.EqType & R) && Result < 0) ret = true;
    return ret;
}

pair<double, double> grad(double x, double y, double delta,vector<Eqelem>& Eq) { // nabla(f(x,y))
    double rx = (calcEq0(x + delta, y, Eq) - calcEq0(x - delta, y, Eq)) / (2 * delta);
    double ry = (calcEq0(x, y + delta, Eq) - calcEq0(x, y - delta, Eq)) / (2 * delta);
    return { rx,ry };
}

double calcEq0(double x,double y,vector<Eqelem>& Eq){ //euclidean
    stack<double> s;
    for (auto item : Eq) {
        if (item.elemType == 0) s.push(item.data0);
        else if (item.elemType == 1) {
            if (item.data1 == 'x') s.push(x);
            else if (item.data1 == 'y') s.push(y);
        }
        else if (item.elemType == 2) {
        }
        else if (item.elemType == 3) {
            double lnum, rnum, res;
            rnum = s.top(); s.pop();
            lnum = s.top(); s.pop();
            res;
            if (item.data3 == '+') res = lnum + rnum;
            else if (item.data3 == '-') res = lnum - rnum;
            else if (item.data3 == '*') res = lnum * rnum;
            else if (item.data3 == '/') res = lnum / rnum;
            else if (item.data3 == '^') res = pow(lnum, rnum);
            s.push(res);
        }
    }
    return s.top();
}

double calcEq1(double Re,double Im, vector<Eqelem>& Eq) { //complex
    complex<double> X = { Re,Im };
    stack<complex<double>> s;
    for (auto item : Eq) {
        if (item.elemType == 0) s.push(item.data0);
        else if (item.elemType == 1) {
            s.push(X);
        }
        else if (item.elemType == 2) {

        }
        else if (item.elemType == 3) {
            complex<double> lnum, rnum, res;
            rnum = s.top(); s.pop();
            lnum = s.top(); s.pop();
            res;
            if (item.data3 == '+') res = lnum + rnum;
            else if (item.data3 == '-') res = lnum - rnum;
            else if (item.data3 == '*') res = lnum * rnum;
            else if (item.data3 == '/') res = lnum / rnum;
            else if (item.data3 == '^') res = pow(lnum, rnum);
            s.push(res);
        }
    }
    return norm(s.top());
}

double norm(pair<double, double> x) {
    return sqrt(x.first * x.first + x.second * x.second);
}