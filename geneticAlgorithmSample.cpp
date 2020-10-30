/*******************************
*Author:榊 統
*Created:2020/10/26
*********************************/

#include <iostream>
#include <algorithm>
#include <thread>
#include <chrono>
#include <vector>
#include <random>
#include <iomanip>
using namespace std;

constexpr int POPULATION_SIZE=500;//個体の数
const string target="Hello, World!! from GeneticAlgorithm";//作りたい文字列

//遺伝子情報
struct genome{
private:
    random_device rnd;
    default_random_engine mt;
    uniform_int_distribution<> randomIntegerGenerator0to9;//0~10の整数をランダムで生成
    uniform_int_distribution<> randomCharGenerator;//0~255の整数をランダムで生成
    uniform_int_distribution<> randomIndexGenerator;//インデックスをランダムに生成
    void init(){
        s.resize(target.size());
        for(int i=0;i<target.size();++i){
            s[i]=randomCharGenerator(mt);
        }
    }        
public:    
    string s;
    int score;

    genome():mt(rnd()),randomIntegerGenerator0to9(0,9),randomCharGenerator(0,255),randomIndexGenerator(0,(int)target.size()-1){
        init();
        score=1000000000;
    }
    genome(const string &t):s(t),mt(rnd()),randomIntegerGenerator0to9(0,9),randomCharGenerator(0,255),randomIndexGenerator(0,(int)target.size()-1){
        score=1000000000;
    }

    genome(const genome &another){
        this->s=another.s;
        this->score=another.score;
    }

    //他の遺伝子のスコアと比較する
    bool operator<(const genome &another){
        return score<another.score;
    }
    bool operator>(const genome &another){
        return score>another.score;
    }
    
    genome operator=(const genome &another){
        this->s=another.s;
        this->score=another.score;
        return *this;
    }

    //この遺伝子の突然変異
    //文字列のある1点の文字を変更
    void mutation(){
        int k=randomIntegerGenerator0to9(mt);
        if(k<2){
            //どこか1箇所の文字を変更する
            s[randomIndexGenerator(mt)]=randomCharGenerator(mt);
        }
        else{
            //何もしない
        }
    }
    //この遺伝子と別の遺伝子との交叉(交配)
    //一様交叉を行う
    void crossover(genome &another){
        for(int i=0;i<target.size();++i){
            if(randomIntegerGenerator0to9(mt)&1){
                //交配によって文字列のi番目を入れ替える
                swap(s[i],another.s[i]);
            }
        }
    }
};
void scoring(vector<genome> &population);
void updateBestGenome(genome &bestGenome,const vector<genome> &population);
void select(vector<genome> &population);
void crossover(vector<genome> &population);
void mutate(vector<genome> &population);
void showBestGenome(const int generation,const genome &bestGenome);
bool isBest(const genome bestGenome);

int main(){
    vector<genome> population(POPULATION_SIZE);//世代全ての遺伝子情報
    genome bestGenome;//これまでの世代で最も良い遺伝子
    int generation=0;//世代
    bool flag=true;

    while(flag){
        sort(population.begin(),population.end());//スコアの低い順にソート
        updateBestGenome(bestGenome,population);//これまでの世代で一番良い遺伝子情報を持つ個体を更新する
        showBestGenome(++generation,bestGenome);//現世代でもっとも良い解を表示
        flag=isBest(bestGenome);//求める遺伝子が得られたらプログラムを終了する
        select(population);//選択と淘汰
        crossover(population);//交叉(交配)
        mutate(population);//突然変異
        scoring(population);//各個体の遺伝子のスコアを評価する
    }
}

//今回は目的の文字列と一致すれば良いので、一致したらflagがfalseになってプログラムが終了する
bool isBest(const genome bestGenome){
    return (bestGenome.s!=target);
}

//各々の個体でスコアを測定する(値が少ない方が良い遺伝子)
void scoring(vector<genome> &population){
    for(int i=0;i<POPULATION_SIZE;++i){
        const string t=population[i].s;
        int score=0;
        for(int j=0;j<target.size();++j){
            //文字が異なるほど最適解から遠い
            score+=(t[j]!=target[j]);
        }
        population[i].score=score;
    }
}

//これまでの世代で最も良い遺伝子を持つものをbestGenomeとする
void updateBestGenome(genome &bestGenome,const vector<genome> &population){
    if(bestGenome>population[0]){
        bestGenome=population[0];
    }
}

//適用度の低いゲノムを淘汰する関数
//適用度が上位50%のものを選択し、下位50%を淘汰する
void select(vector<genome> &population){
    for(int i=0;i<POPULATION_SIZE/2;++i){
        population[i+(POPULATION_SIZE/2)]=population[i];
    }
}

//遺伝子交配(交叉)をする関数
void crossover(vector<genome> &population){
    for(int i=POPULATION_SIZE/2+1;i<POPULATION_SIZE;++i){
        population[i].crossover(population[i-1]);
    }
}

//突然変異をする関数
void mutate(vector<genome> &population){
    for(int i=POPULATION_SIZE/2;i<POPULATION_SIZE;++i){
        population[i].mutation();
    }
}

//現状最も良い遺伝子情報を持つものを表示する関数
void showBestGenome(const int generation,const genome &bestGenome){
    cout<<right<<setw(3)<<generation<<"世代目の最も良い遺伝子:"<<bestGenome.s<<endl;
}