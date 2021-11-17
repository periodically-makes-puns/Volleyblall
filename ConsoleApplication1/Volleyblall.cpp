#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <string>
#include <limits>
#include <format>
#include <cassert>
#include <queue>
#include <set>

#pragma execution_character_set( "utf-8" )
#define MAX_EXT numeric_limits<streamsize>::max()
#define AUTO true

using namespace std;

bool silent = true;

void dump_groups(vector<vector<string>>& groups) {
    for (vector<string>& group : groups) {
        for (string& s : group) {
            cout << s << endl;
        }
        cout << endl;
    }
}

template <typename... Args>
void print(const string& fmt, Args&&... args) {
    if (!silent)
        cout << vformat(fmt, make_format_args(args...)) << endl;
}

unsigned char _w[17];
unsigned char _l[17];
unsigned char _r[17];
char _sd[17];
unsigned char _n[17][17];

unsigned char w[17];
unsigned char l[17];
unsigned char r[17];
char sd[17];
unsigned char n[17][17];

typedef struct Result {
    unsigned char w;
    char sd;
    unsigned char l;
    string* name;
    int orig;
} Result;

Result subgroup[17];
vector<long long> fancyW;


int main(int argc, char** argv)
{   
    bool cont = AUTO;
    if (argc >= 2 && argv[1] == "auto") cont = true;
    vector<vector<string>> groups;
    vector<string> names;
    ifstream gfile("groups.txt");
    ifstream gsfile("group_stage.txt");
    ofstream sfile("poss_seeds.csv");
    vector<string>* group = new vector<string>();
    string line;
    while (!gfile.eof()) {
        getline(gfile, line);

        if (line.length() == 0) {
            groups.push_back(*group);
            group = new vector<string>();
        } else if (line[0] == '>' && line[1] == ' ') {
            cout << "Initializing group " << line.substr(2) << endl;
            names.push_back(line.substr(2));
        } else {
            group->push_back(line);
        }
    }

    int game = 0;
    for (vector<string> group : groups) {
        memset(_w, 0, sizeof(_w));
        memset(_l, 0, sizeof(_l));
        memset(_r, 0, sizeof(_r));
        memset(_sd, 0, sizeof(_sd));
        memset(_n, -1, sizeof(_n));
        sort(group.begin(), group.end());
        assert(is_sorted(group.begin(), group.end()));
        cout << endl;
        for (unsigned int _ = 0; _ < group.size() * (group.size() - 1) / 2; _++) {
            (void) _;
            size_t left, right;
            string name1, name2, scoreline;
            getline(gsfile, line);
            gsfile.ignore(MAX_EXT, '\n');
            gsfile.ignore(MAX_EXT, '\n');
            gsfile.ignore(MAX_EXT, '\n');

            if (line.substr(0, 7) == "> MATCH") {
                left = 10;
                right = line.find(" | ", left);
                name1 = line.substr(left, right - left);
                left = right + 3;
                right = line.find(" | ", left);
                name2 = line.substr(left, right - left);
                scoreline = line.substr(right + 3);
                vector<string>::iterator it = lower_bound(group.begin(), group.end(), name1);
                vector<string>::iterator jt = lower_bound(group.begin(), group.end(), name2);
                __int64 i = it - group.begin(), j = jt - group.begin();
                if (!binary_search(group.begin(), group.end(), name1) ||
                    !binary_search(group.begin(), group.end(), name2)) {
                    cerr << format("somethings fucked {} {}", i, j) << endl;
                    cerr << format("{} {} {}", names[game], name1, name2) << endl;
                    cerr << is_sorted(group.begin(), group.end()) << endl;
                    return -1;
                }
                //cout << format("{} {}", i, j) << endl;
                //cout << name1 << " v. " << name2 << " & " << scoreline << endl;
                if (scoreline.substr(0, 3) != "Yet") {
                    if (scoreline[0] < scoreline[2]) {
                        _w[j]++;
                        _l[i]++;
                        _sd[j] += scoreline[2] - scoreline[0];
                        _sd[i] -= scoreline[2] - scoreline[0];
                        _n[i][j] = _n[j][i] = 0;
                    } else {
                        _w[i]++;
                        _l[j]++;
                        _sd[j] += scoreline[2] - scoreline[0];
                        _sd[i] -= scoreline[2] - scoreline[0];
                        _n[i][j] = _n[j][i] = 0;
                    }
                } else {
                    _n[i][j] = _n[j][i] = 1;
                    _r[i]++;
                    _r[j]++;
                }
            }
        }
        print("START {}", names[game++]);
        int N = (int) group.size();
        for (int i = 0; i < N; i++) {
            _n[i][i] = 0;
            for (int j = 0; j < N; j++) {
                assert((_n[i][j] == 0) || (_n[i][j] == 1));
            }
            print("{:40} {:2} {:2} {:2d} {:2}", group[i], _w[i], _l[i], _sd[i], _r[i]);
            assert(_w[i] + _l[i] + _r[i] == N - 1);
        }
        for (int i = 0; i < N; i++) {
            memcpy(w, _w, sizeof(w));
            memcpy(l, _l, sizeof(l));
            memcpy(r, _r, sizeof(r));
            memcpy(sd, _sd, sizeof(sd));
            memcpy(n, _n, sizeof(n));
            sfile << group[i] + "|";

            print("Optimizing for {}", group[i]);
            for (int j = 0; j < N; j++) {
                if (n[i][j] > 0) {
                    w[i] += n[i][j];
                    l[j] += n[i][j];
                    r[i] -= n[i][j];
                    r[j] -= n[i][j];
                    sd[i] += 2;
                    sd[j] -= 2;
                    n[i][j] = n[j][i] = 0;
                }
            }
            queue<int> q;
            unsigned char high = 0, low = 0;
            for (int j = 0; j < N; j++) {
                if (i == j) continue;
                if (w[j] > w[i] || (w[j] == w[i] && sd[j] - r[j] * 2 > sd[i])) {
                    q.push(j);
                } 
                if (l[j] > l[i] || (l[j] == l[i] && r[j] * 2 + sd[j] <= sd[i])) {
                    q.push(j);
                    if (r[j] == 0) {
                        print("OVER {}", group[j]);
                    }
                }
            }
            // initializers
            while (!q.empty()) {
                int t = q.front();
                q.pop();
                if (r[t] == 0) continue;
                //print("HANDLING {} {}", group[t], r[t]);
                for (int k = 0; k < N; k++) {
                    if (n[k][t] > 0) {
                        l[k] += n[k][t];
                        w[t] += n[k][t];
                        r[k] -= n[k][t];
                        r[t] -= n[k][t];
                        sd[k] -= 2;
                        sd[t] += 2;
                        n[k][t] = n[t][k] = 0;
                        if (l[k] > l[i] || (l[k] == l[i] && r[k] * 2 + sd[k] <= sd[i])) {
                            q.push(k);
                        }
                    }
                }
            }
            // Mark all players strictly above or below this player.
            int sgN = 0;
            for (int j = 0; j < N; j++) {
                if (i != j) {
                    if (r[j] == 0) {
                        if (w[j] > w[i] || (w[j] == w[i] && sd[j] > sd[i])) high++;
                        else if (w[j] < w[i] || (w[j] == w[i] && sd[j] <= sd[i])) low++;
                    } else {
                        subgroup[sgN++] = {
                            .w = w[j],
                            .sd = sd[j],
                            .l = l[j],
                            .name = &group[j],
                            .orig = j
                        };
                    }
                }
                print("{:40} {:2} {:2} {:2d} {:2}", group[j], w[j], l[j], sd[j], r[j]);
            } 
            // Display group results & cons subgroup
            if (sgN > 0) {
                // Oh boy here we go :D
                print("The case of {} requires more analysis. Continue?", group[i]);
                bool conti = cont;

                if (!conti) {
                    char tmp;
                    cout << "> ";
                    cin >> tmp;
                    conti = tmp == 'y';
                }
                if (conti) {
                    int wins = w[i];
                    fancyW.clear();
                    print("Subgroup size: {}", sgN);
                    for (long long bs = 0; bs < (1LL << sgN); bs++) {
                        int lhs = 0, lhs2 = 0, rhs = 0;
                        for (int j = 0; j < sgN; j++) {
                            if ((1LL << j) & bs) {
                                lhs += wins - subgroup[j].w;
                                lhs2 += wins - subgroup[j].w - 1;
                                for (int k = j + 1; k < sgN; k++) {
                                    if ((1LL << k) & bs) rhs += n[subgroup[j].orig][subgroup[k].orig];
                                }
                            }
                        }
                        print("{:0" + to_string(sgN) + "b} {}, {} {} {}",
                            bs, (lhs < rhs) ? "passes" : "fails",
                            lhs, (lhs < rhs) ? "<" : ">=", rhs);
                        if (lhs < rhs) {
                            fancyW.push_back(bs);
                        }
                    }
                    if (fancyW.size() == 0) {
                        print("{} can still reach first in the subgroup. Continuing.", group[i]);
                    } else {
                        long long minPass = -1;
                        int minN = sgN + 1;
                        for (long long bsH = 0; bsH < (1LL << sgN); bsH++) {
                            bool pass = true;
                            for (long long bs : fancyW) {
                                if ((bs & bsH) == 0LL) {
                                    pass = false;
                                    break;
                                }
                            }
                            if (pass) {
                                int HN = 0;
                                for (int j = 0; j < sgN; j++) HN += (bsH & (1LL << j)) ? 1 : 0;
                                if (HN < minN) {
                                    minN = HN;
                                    minPass = bsH;
                                }
                            }
                        }
                        high += minN;
                    }
                }
                if (!cont) {
                    string _;
                    cout << "Analysis complete. Pausing for user review.";
                    cin >> _;
                }
            }
            string suffix;
            int place = high + 1;
            if (place % 10 == 1 && place % 100 != 11) {
                suffix = "st";
            } else if (place % 10 == 2 && place % 100 != 12) {
                suffix = "nd";
            } else if (place % 10 == 3 && place % 100 != 13) {
                suffix = "rd";
            } else {
                suffix = "th";
            }
            cout << format("The best {} can do is {}{}", group[i],
                place, suffix) << endl;
            sfile << place << "|";
            memcpy(w, _w, sizeof(w));
            memcpy(l, _l, sizeof(l));
            memcpy(r, _r, sizeof(r));
            memcpy(sd, _sd, sizeof(sd));
            memcpy(n, _n, sizeof(n));

            print("Optimizing against {}", group[i]);

            for (int j = 0; j < N; j++) {
                if (n[i][j] > 0) {
                    w[j] += n[i][j];
                    l[i] += n[i][j];
                    r[i] -= n[i][j];
                    r[j] -= n[i][j];
                    sd[j] += 2;
                    sd[i] -= 2;
                    n[i][j] = n[j][i] = 0;
                }
            }
            high = 0, low = 0;
            for (int j = 0; j < N; j++) {
                if (i == j) continue;
                if (w[j] > w[i] || (w[j] == w[i] && sd[j] - r[j] * 2 > sd[i])) {
                    q.push(j);
                }
                if (l[j] > l[i] || (l[j] == l[i] && r[j] * 2 + sd[j] <= sd[i])) {
                    q.push(j);
                }
            }
            while (!q.empty()) {
                int t = q.front();
                q.pop();
                if (r[t] == 0) continue;
                //print("HANDLING {} {}", group[t], r[t]);
                for (int k = 0; k < N; k++) {
                    if (n[k][t] > 0) {
                        l[t] += n[k][t];
                        w[k] += n[k][t];
                        r[k] -= n[k][t];
                        r[t] -= n[k][t];
                        sd[t] -= 2;
                        sd[k] += 2;
                        n[k][t] = n[t][k] = 0;
                        if (l[k] > l[i] || (l[k] == l[i] && r[k] * 2 + sd[k] <= sd[i])) {
                            q.push(k);
                        }
                    }
                }
            }
            // Mark all players strictly above or below this player.
            sgN = 0;
            for (int j = 0; j < N; j++) {
                if (i != j) {
                    if (r[j] == 0) {
                        if (w[j] > w[i] || (w[j] == w[i] && sd[j] > sd[i])) high++;
                        else if (w[j] < w[i] || (w[j] == w[i] && sd[j] <= sd[i])) low++;
                    } else {
                        subgroup[sgN++] = {
                            .w = w[j],
                            .sd = sd[j],
                            .l = l[j],
                            .name = &group[j],
                            .orig = j
                        };
                    }
                }
                print("{:40} {:2} {:2} {:2d} {:2}", group[j], w[j], l[j], sd[j], r[j]);
            }
            // Display group results & cons subgroup
            if (sgN > 0) {
                // Oh boy here we go :D
                print("The case of {} requires more analysis. Continue?", group[i]);
                bool conti = cont;

                if (!conti) {
                    char tmp;
                    cout << "> ";
                    cin >> tmp;
                    conti = tmp == 'y';
                }
                if (conti) {
                    int wins = l[i];
                    fancyW.clear();
                    print("Subgroup size: {}", sgN);
                    for (long long bs = 0; bs < (1LL << sgN); bs++) {
                        int lhs = 0, lhs2 = 0, rhs = 0;
                        for (int j = 0; j < sgN; j++) {
                            if ((1LL << j) & bs) {
                                lhs += wins - subgroup[j].l;
                                lhs2 += wins - subgroup[j].l - 1;
                                for (int k = j + 1; k < sgN; k++) {
                                    if ((1LL << k) & bs) rhs += n[subgroup[j].orig][subgroup[k].orig];
                                }
                            }
                        }
                        print("{:0" + to_string(sgN) + "b} {}, {} {} {}",
                            bs, (lhs < rhs) ? "passes" : "fails",
                            lhs, (lhs < rhs) ? "<" : ">=", rhs);
                        if (lhs < rhs) {
                            fancyW.push_back(bs);
                        }
                    }
                    if (fancyW.size() == 0) {
                        print("{} can still reach last in the subgroup. Continuing.", group[i]);
                    } else {
                        long long minPass = -1;
                        int minN = sgN + 1;
                        for (long long bsH = 0; bsH < (1LL << sgN); bsH++) {
                            bool pass = true;
                            for (long long bs : fancyW) {
                                if ((bs & bsH) == 0LL) {
                                    pass = false;
                                    break;
                                }
                            }
                            if (pass) {
                                int HN = 0;
                                for (int j = 0; j < sgN; j++) HN += (bsH & (1LL << j)) ? 1 : 0;
                                if (HN < minN) {
                                    minN = HN;
                                    minPass = bsH;
                                }
                            }
                        }
                        low += minN;
                    }
                }
                if (!cont) {
                    string _;
                    cout << "Analysis complete. Pausing for user review.";
                    cin >> _;
                }
            }
            place = N - low;
            if (place % 10 == 1 && place % 100 != 11) {
                suffix = "st";
            } else if (place % 10 == 2 && place % 100 != 12) {
                suffix = "nd";
            } else if (place % 10 == 3 && place % 100 != 13) {
                suffix = "rd";
            } else {
                suffix = "th";
            }
            cout << format("The worst {} can do is {}{}", group[i],
                place, suffix) << endl;
            sfile << place << "|";
            if (10 > N || N > 16) {
                sfile << ((N == place) ? "FALSE" : "TRUE") << endl;
            } else {
                sfile << "TRUE" << endl;
            }
        }
    }
}
