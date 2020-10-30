/*******************************
*Author:榊 統
*Created:2020/10/28
*********************************/

#include <iostream>
#include <algorithm>
#include <thread>
#include <chrono>
#include <vector>
#include <random>
#include <iomanip>
using namespace std;

constexpr int POPULATION_SIZE=1000;//個体の数
constexpr int MAX_FUNCTION_SIZE=10;//目的関数の次元数
const double W=0.77;//慣性W
const double Cp=0.77;//ハイパーパラメータCp
const double Cg=0.76;//ハイパーパラメータCg

//各particleの情報
struct particle{
private:
    random_device rnd;
    mt19937 mt;
    uniform_int_distribution<> randomInteger;//0~1e9の整数をランダムで生成
    double randomDouble(){
        double res=randomInteger(mt);
        res/=1e8;
        if(randomInteger(mt)&1){
            res*=-1;
        }
        return res;
    }
    double randomDouble0to1(){
        double res=randomInteger(mt);
        res/=1e9;
        return res;
    }
    void init(){
        now.resize(MAX_FUNCTION_SIZE);
        personalBest.resize(MAX_FUNCTION_SIZE);
        globalBest.resize(MAX_FUNCTION_SIZE);
        v.resize(MAX_FUNCTION_SIZE);
        personalBestScore=1e18;

        for(int i=0;i<MAX_FUNCTION_SIZE;++i){
            now[i]=randomDouble();
            personalBest[i]=randomDouble();
            globalBest[i]=randomDouble();
            v[i]=randomDouble();
        }
    }
public:
    vector<double> now;//現在地点
    vector<double> personalBest;//このparticleの過去の探索地点のうちもっともよい点。
    vector<double> globalBest;//全てのparticleの過去の探索地点のうちもっともよい点。
    double personalBestScore;//このparticleの過去の探索地点のうち最も良いスコア
    vector<double> v;//このparticleの速度ベクトル

    particle():mt(rand()),randomInteger(0,1e9){
        init();
    }

    //他の遺伝子のスコアと比較する
    bool operator<(const particle &another){
        return personalBestScore<another.personalBestScore;
    }
    bool operator>(const particle &another){
        return personalBestScore>another.personalBestScore;
    }

    void next(){
        for(int i=0;i<MAX_FUNCTION_SIZE;++i){
            now[i]+=v[i];
        }
        const double r1=randomDouble0to1();
        const double r2=randomDouble0to1();
        for(int i=0;i<MAX_FUNCTION_SIZE;++i){
            v[i]=W*v[i]+Cp*r1*(personalBest[i]-now[i])+Cg*r2*(globalBest[i]-now[i]);
        }
    }

    void reborn(){
        for(int i=0;i<MAX_FUNCTION_SIZE;++i){
            v[i]=randomDouble();
        }
    }
};

struct io_init{
    io_init(){
        cout<<fixed<<setprecision(12);
    }
}ALL_INIT;

bool isBest(const vector<double> &bestAnswer);
void scoring(vector<particle> &population);
void showBestParticle(const int generation,const vector<double> &bestAnswer,const double &bestScore);
void particleMove(vector<particle> &population);
void updateBestParticle(vector<particle> &population,vector<double> &bestAnswer,double &bestScore);

int main(){
    vector<particle> population(POPULATION_SIZE);
    vector<double> bestAnswer;//これまでの世代で最も良い遺伝子
    double bestScore=1e18;//これまでの世代でもっともよい遺伝子のスコア
    int generation=0;//個体の世代
    scoring(population);
    bool flag=true;

    while(flag){
        updateBestParticle(population,bestAnswer,bestScore);
        showBestParticle(++generation,bestAnswer,bestScore);//現世代でもっとも良い解を表示
        flag=isBest(bestAnswer);
        particleMove(population);//各particleを動かす
        scoring(population);//各個体の遺伝子のスコアを評価する
    }

    return 0;
}

//最適解が見つかったら終了する
bool isBest(const vector<double> &bestAnswer){
    double miss=20+exp(1.0);
    double powSum=0.0;
    double cosSum=0.0;
    for(int i=0;i<MAX_FUNCTION_SIZE;++i){
        powSum+=pow(bestAnswer[i],2);
        cosSum+=cos(2*M_PI*bestAnswer[i]);
    }
    miss-=20*exp(-0.2*sqrt(powSum/MAX_FUNCTION_SIZE));
    miss-=exp(cosSum/MAX_FUNCTION_SIZE);
    return (miss>1e-9);
}

//各々の個体でスコアを測定する(少ない方が良い遺伝子)
void scoring(vector<particle> &population){
    static const int targetLength=MAX_FUNCTION_SIZE;
    for(int i=0;i<POPULATION_SIZE;++i){
        vector<double> t=population[i].now;
        double score=20+exp(1.0);
        double powSum=0.0;
        double cosSum=0.0;
        for(int j=0;j<targetLength;++j){
            powSum+=pow(t[j],2);
            cosSum+=cos(2*M_PI*t[j]);
        }
        score-=20*exp(-0.2*sqrt(powSum/MAX_FUNCTION_SIZE));
        score-=exp(cosSum/MAX_FUNCTION_SIZE);
        if(population[i].personalBestScore>score){
            population[i].personalBestScore=score;
            population[i].personalBest=t;
        }
    }
}

//現状最も良い解を表示する関数
void showBestParticle(const int generation,const vector<double> &bestAnswer,const double &bestScore){
    cout<<bestScore<<endl;
    cout<<right<<setw(3)<<generation<<"世代目の最も良い遺伝子: {";
    for(int i=0;i<MAX_FUNCTION_SIZE;i++){
        if(i!=0) cout<<" ";
        cout<<bestAnswer[i];
    }
    cout<<"}"<<endl;
}

//各particleを次の探索場所へ移動させる
void particleMove(vector<particle> &population){
    for(int i=0;i<POPULATION_SIZE;++i){
        population[i].next();
    }
}

//一番良い解の更新とグローバルベストの更新
void updateBestParticle(vector<particle> &population,vector<double> &bestAnswer,double &bestScore){
    double score=1e18;
    vector<double> best(MAX_FUNCTION_SIZE);
    for(int i=0;i<POPULATION_SIZE;++i){
        if(score>population[i].personalBestScore){
            score=population[i].personalBestScore;
            best=population[i].personalBest;
        }
    }

    for(int i=0;i<POPULATION_SIZE;++i){
        population[i].globalBest=best;
    }

    bestScore=score;
    bestAnswer=best;
}