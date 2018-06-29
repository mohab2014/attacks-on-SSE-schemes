

#include <cstdlib>


#include <cstdio>

#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <ctime>

#include <string> 
#include <set>
#include <stdlib.h>

#include <iterator>
#include <sstream>




#include <omp.h>


using namespace std;

int target_cardinality = 2;
int number_of_records = 0;
int maxNoQueries = 0;

int** Ct; //[maxNoQueries+1][maxNoQueries+1] = {{0}};
int no_solutions = 0;

bool** dp;





class Query
{
public:
    Query()
    {
        query_no = 0;
        result_size = 0;
        tag = 0;
    }
    int query_no;
    int result_size;
    int tag;
    vector<Query> queryList;
};

class CompResultSize
{
  public:
    inline bool operator() (const Query& q1, const Query& q2)
    {
        return (q1.result_size < q2.result_size);
    }
};


class CompQueryNo
{
  public:
    inline bool operator() (const Query& q1, const Query& q2)
    {
        //if(q1.result_size == q2.result_size)    
           return (q1.query_no < q2.query_no);
        
       //return (q1.result_size < q2.result_size);
        
    }
};

class CompQueryListSize
{
  public:
    inline bool operator() (const Query& q1, const Query& q2)
    {
        return (q1.queryList.size() < q2.queryList.size());
    }
};


class Attribute
{
public:
    string name;
    int cardinality;
    //int attempts_to_resolve;
    string solutionQueryString;
    int confirmations;
    vector<vector<Query>> solutions;
    //int repeated;
};


class IncompleteSolution
{
public:
    vector<Query> intersection;
    vector<Query> solutions[2];
    int card;
 
};

 

vector<vector<Query>> correctSolutions;


//bool operator< (Query qi,Query qj) { int xi,xj; if(qi.queryList == NULL) xi = 0; else xi = qi.queryList->size(); if(qj.queryList==NULL) xj=0; else xj = qj.queryList->size(); 

bool operator< (Query qi,Query qj) 
{ 
    int xi,xj; 
    
    if(qi.queryList.size() == 0) 
        xi = 0; 
    else 
        xi = qi.queryList.size(); 
    
    if(qj.queryList.size()==0) 
        xj=0; 
    else 
        xj = qj.queryList.size(); 

    if(xi == 0 || xj == 0 || xi==xj)
     return qi.result_size < qj.result_size; 
    
   return xi<xj;
}

//std::ostream& operator<<(std::ostream& out, const Query& q){ if(q.queryList == NULL) out<<"(0,"<<q.query_no<<","<<q.result_size<<")"; else out<<"("<<q.queryList->size()<<","<<q.query_no<<","<<q.result_size<<")";}


std::ostream& operator<<(std::ostream& out, const Query& q)
{ 
    
    if(q.queryList.size() == 0) 
        out<<"(0,"<<q.query_no<<","<<q.result_size<<")"; 
    else 
        out<<"("<<q.queryList.size()<<","<<q.query_no<<","<<q.result_size<<")";


}

bool operator== (Query qi,Query qj) { return (qi.query_no==qj.query_no) && (qi.result_size == qj.result_size);}

bool isQueryRemoved(Query q){return (q.query_no == -1);}
bool isEqual(Query qi,Query qj) { return (qi.query_no==qj.query_no && qi.result_size == qj.result_size);}

bool checkQueryExist(const Query& q, const vector<Query>& vq)
{
    
    for(int i = 0;i < vq.size();i++)
        if(vq[i] == q)
            return true;
    return false;
}

//-------------------------------------------------------------------------------------------
int queryExistBS1(const Query& q, const vector<Query>& vq, int l, int r)
{
    
    
   if (r >= l)
   {
        int mid = l + (r - l)/2;
 
        // If the element is present at the middle itself
        if (vq[mid].query_no == q.query_no)  
            return mid;
 
        // If element is smaller than mid, then it can only be present
        // in left subarray
        if (vq[mid].query_no > q.query_no) 
            return queryExistBS1(q, vq, l, mid-1);
 
        // Else the element can only be present in right subarray
        return queryExistBS1(q, vq, mid+1, r);
   }
 
   // We reach here when element is not present in array
   return -1;
    
    
}

bool isClose(int confirmations, int card)
{
    
   
    if(confirmations == card)
        return true;
    
    if(card <= 15)
    {
        if(confirmations == card)
            return true;
    }
    else
    {
        if(confirmations >= 10)
            return true;
    }
    
    
    return false;
}




//-----------------------------------------------------------------------------

string itos(int i) // convert int to string
{
    stringstream s;
    s << i;
    return s.str();
}

//------------------------------------------------------------------------------

vector<Query> intersection(const vector<Query>& v1,const vector<Query>& v2)
{
    vector<Query> v1v2;
    
    
    for(int i = 0;i < v1.size();i++)
        for(int j = 0;j < v2.size();j++)
            if(v1[i] == v2[j])
                v1v2.push_back(v1[i]);
        
        
    return v1v2;
    
}


//--------------------------------------------------------------------------------
vector<Query> efficientIntersection(vector<Query>& vSmall,const vector<Query>& vBig)
{
    vector<Query> result;
    
    
    sort(vSmall.begin(),vSmall.end(),CompQueryNo()); //sorting according to query no since query result size is repetitive
    
    for(int i = 0;i < vBig.size();i++)
         if(queryExistBS1(vBig[i],vSmall,0,vSmall.size()-1) != -1)
                result.push_back(vBig[i]);
        
        
    return result;
    
}

//-------------------------------------------------------------------------------

bool testZeroCo_Occurrence(const vector<Query>& v)
{
        for(int i = 0;i < v.size();i++)
        {
            for(int j = i+1;j < v.size();j++)
            {
                if(Ct[v[i].query_no][v[j].query_no] != 0)
                {
                    return false;
                }
            }
        }
        
        
        return true;
}


//------------------------------------------------------------------------
int findDegree(const vector<Query> &List, const Query &q)
{
    int degree  = 0;
    
    for(int i = 0;i < List.size();i++)
        if(List[i].query_no != q.query_no && Ct[List[i].query_no][q.query_no] != 0)
            degree++;
    
    return degree;
}
//-------------------------------------------------------------------------------
Query minimumDegreeQuery(const vector<Query> &List, int maxNo)
{
    Query result;
    
    result.query_no = 0;
    result.tag = maxNo-1;
    
    for(int i = 1;i < List.size();i++)
    {
        
        int degree = findDegree(List, List[i]);
        
        cout<<"degree = "<<degree<<endl;
        
        if(degree < result.tag)
        {
            result.tag = degree;
            result.query_no = List[i].query_no;
            result.result_size = List[i].result_size;
            
            
        }
            
    }
    
    return result;
    
    
}
//-------------------------------------------------------------------------------
Query maximumDegreeQuery(const vector<Query> &List)
{
    Query result;
    
    result.query_no = -1;
    result.tag = -1;
    
    
    if(List.size() == 1)
        return List[0];
    
    for(int i = 1;i < List.size();i++)
    {
        
        int degree = findDegree(List, List[i]);
        
        //cout<<"max degree = "<<degree<<endl;
        
        if(degree > result.tag)
        {
            result.tag = degree;
            result.query_no = List[i].query_no;
            result.result_size = List[i].result_size;
            
            
        }
            
    }
    
    //cout<<"max degree = "<<result.tag<<endl;
    return result;
    
    
}
//-------------------------------------------------------------------------------
Query maximumResultSizeQuery(const vector<Query> &List)
{
    Query result;
    
    result.query_no = -1;
    result.result_size = -1;
    
    
    if(List.size() == 1)
        return List[0];
    
    for(int i = 1;i < List.size();i++)
    {
        

        if(List[i].result_size > result.result_size)
        {
            
            result.query_no = List[i].query_no;
            result.result_size = List[i].result_size;
            
        }
            
    }
    
    //cout<<"max degree = "<<result.tag<<endl;
    return result;
    
    
}
//-------------------------------------------------------------------------------
vector<Query> independentSetGreedyMin(vector<Query> List, int maxNo, int n)
{
    
    vector<Query> S;
    Query head;
    
    if(List.size() > 0)
    {
        head = List[0];
        S.push_back(head);
    
    }
    
    while(List.size() > 1)
    {
        
        Query chosenQuery =  minimumDegreeQuery(List,maxNo);//maximumDegreeQuery(List);//
        
         
        if(chosenQuery.tag == 0)
            return S;
        //remove result.query_no and its adjacent queries from L
        S.push_back(chosenQuery);
        
        vector<Query> newReducedList;
        newReducedList.push_back(head);
        for(int i = 1;i < List.size();i++)
            if(List[i].query_no != chosenQuery.query_no && Ct[List[i].query_no][chosenQuery.query_no] == 0 && (S[0].result_size + List[i].result_size+chosenQuery.result_size <= n))
                newReducedList.push_back(List[i]);
        
        
        List.clear();
        List = newReducedList;
    }
    
    
    
    return S;
    
    
}

//-------------------------------------------------------------------------------
vector<Query> independentSetGreedyMax(vector<Query> List, int n)
{
  
    
    
    vector<Query> S;
    
    
    Query head;
    
    if(List.size() > 0)
    {
        head = List[0];
        S.push_back(head);
    
    }
    
    while(List.size() > 1)
    {
        
        Query maxQuery =  maximumDegreeQuery(List);//minimumDegreeQuery(List,maxNo);
        
        //if minQuery exists in List
        //remove result.query_no and its adjacent queries from L
        //cout<<maxQuery.tag<<","<<maxQuery.query_no<<","<<List.size()<<" ";
        
        if(maxQuery.tag == 0)
            return S;
        
        S.push_back(maxQuery);
        
        vector<Query> newReducedList;
        newReducedList.push_back(head);
        for(int i = List.size()-1;i>=0;i--)
            if(List[i].query_no != maxQuery.query_no && Ct[List[i].query_no][maxQuery.query_no] == 0 && (S[0].result_size + List[i].result_size+maxQuery.result_size <= n))
                newReducedList.push_back(List[i]); //List.erase(List.begin()+i);///
        
        
        List.clear();
        List = newReducedList;
    }
    
    return S;
    
    
}
//-------------------------------------------------------------------------------
vector<Query> independentSetGreedyMaxSize(vector<Query> List, int n)
{
  
    
    
    vector<Query> S;
    
    
    Query head;
    
    if(List.size() > 0)
    {
        head = List[0];
        S.push_back(head);
    
    }
    
    while(List.size() > 1)
    {
        
        Query maxQuery =  maximumResultSizeQuery(List);//minimumDegreeQuery(List,maxNo);
        
        //if minQuery exists in List
        //remove result.query_no and its adjacent queries from L
        //cout<<maxQuery.tag<<","<<maxQuery.query_no<<","<<List.size()<<" ";
        
        //if(maxQuery.tag == 0)
          //  return S;
        
        S.push_back(maxQuery);
        
        vector<Query> newReducedList;
        newReducedList.push_back(head);
        for(int i = List.size()-1;i>=0;i--)
            if(List[i].query_no != maxQuery.query_no && Ct[List[i].query_no][maxQuery.query_no] == 0 && (S[0].result_size + List[i].result_size+maxQuery.result_size <= n))
                newReducedList.push_back(List[i]); //List.erase(List.begin()+i);///
        
        
        List.clear();
        List = newReducedList;
    }
    
    return S;
    
    
}
//-------------------------------------------------------------------------------

int isValidCardinality(vector<Attribute> a, int card)
{
    for(int i = 0;i < a.size();i++)
        if(a[i].cardinality == card)
            return i;
    return -1;
}

//----------------------------------------------------------------------------------
int sumResultSizes(vector<Query>& v, int begin, int end)
{
    int sum = 0;
    
    for(int i = begin; i <= end;i++)
        sum += v[i].result_size;
    
    return sum;
}
//------------------------------------------------------------------------------------

vector<Query> subQueryVector(vector<Query> v, int begin, int end)
{
    
    vector<Query> result;
    
    
    for(int i = begin; i <= end;i++)
        result.push_back(v[i]);
    
    return result;
    
}


//-------------------------------------------------------------------------------------
string toString(const vector<Query>& v)
{
      string result = "";
    
      for (int i = 0; i < v.size(); ++i)
             result += (itos(v[i].query_no)+":"+itos(v[i].result_size)+";");
      
      return result;
}
//--------------------------------------------------------------------------------------

void display(const vector<Query>& v)
{ 
      for (int i = 0; i < v.size(); ++i)
             cout<<v[i].query_no<<":"<<v[i].result_size<<";";
      cout<<endl;
          
}

//-------------------------------------------------------------
void Tokenize(const string& str,
                      vector<string>& tokens,
                      const string& delimiters = " ")
{
    // Skip delimiters at beginning.
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    string::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (string::npos != pos || string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}


//-----------------------------------------------------------------------------------
void saveCorrectSolution(vector<Query>& v, vector<Attribute> &targetAttributes) {

    int index = isValidCardinality(targetAttributes,v.size());   
    if(index != -1)//if(v.size() >= target_cardinality)
    {
        
        if(testZeroCo_Occurrence(v)==true)       
          correctSolutions.push_back(v);
       
        
    }
}



//-----------------------------------------------------------------------------------------

void output(const vector<Query>& queryList, int i, int sum, vector<Query>& p, vector<Attribute> targetAtt) 
{
    
    if (i == 0 && sum != 0 && dp[0][sum]) {
        p.push_back(queryList[i]);
          
        saveCorrectSolution(p,targetAtt);
        
        return;
    }
    if (i == 0 && sum == 0) {
        saveCorrectSolution(p,targetAtt);
        return;
    }
    
    if (sum == 0) {
        saveCorrectSolution(p,targetAtt);
        return;
    }
    
    
    if (dp[i - 1][sum]) {
        vector<Query> b = p;
        output(queryList, i - 1, sum, b,targetAtt);
    }
    if (sum >= queryList[i].result_size && dp[i - 1][sum - queryList[i].result_size]) {
        p.push_back(queryList[i]);
        
        if(testZeroCo_Occurrence(p)==true)
            output(queryList, i - 1, sum - queryList[i].result_size, p,targetAtt);
    }
}


//---------------------------------------------------------------------------------------------------
bool subsetSum(vector<Query>& queryList, int sum) 
{
  
    if (queryList.size() == 0 || sum < 0) 
        return false;
    if(dp != NULL)
    {
        delete dp;
        dp = NULL;
    }
    dp = new bool*[queryList.size()];
    for (int i = 0; i < queryList.size(); ++i) {
        dp[i] = new bool[sum + 1];
        dp[i][0] = true;
    }
    for (int i = 1; i < sum + 1; ++i)
        dp[0][i] = (queryList[0].result_size == i) ? true : false;
    for (int i = 1; i < queryList.size(); ++i)
        for (int j = 0; j < sum + 1; ++j)
            dp[i][j] = (queryList[i].result_size <= j) ? dp[i - 1][j] || dp[i - 1][j - queryList[i].result_size] : dp[i - 1][j];
    
    if (!dp[queryList.size() - 1][sum]) {
        return false;
    }
    
    return true;
    
    
}

//------------------------------------------------------------------------------


/*
 * 
 */
int main(int argc, char** argv) 
{ 


    

    int x, y, f; //read the three integers, queries number x and y and their joint frequency value f 
    char c1, c2; //read two commas


           
    
    
    
    
    if(argc != 8)
    {

       std::cout<<"./heuristicattack begin-query-no end-query-no maxNoQueries observedMatrixFile no_records ResultFileName meta_data_info\n";
       exit(1);
    }

    
    int begin_index = atoi(argv[1]);
    
    int end_index = atoi(argv[2]);
    
    maxNoQueries = atoi(argv[3]);
    
    std::ifstream infile(argv[4]);
    
    number_of_records = atoi(argv[5]);
    
    std::ofstream outMetaFile(argv[6]);
    
    std::ifstream metaDataInfo(argv[7]);
    
    //std::ofstream outfile("result.txt");
    
    Ct = new int*[maxNoQueries+1];
    for(int i = 0; i < maxNoQueries+1; ++i) 
    {
       Ct[i] = new int[maxNoQueries+1];
    }
    
    
    for(int i = 0;i < maxNoQueries+1;i++)
        for(int j = 0;j < maxNoQueries+1;j++)
            Ct[i][j] = 0;
    
    while (infile >> x >> c1 >> y >> c2 >> f)
    {

      Ct[x][y] = f;
      Ct[y][x] = f;
      
      //cout<<"Ct["<<x<<"]["<<y<<"] = "<<Ct[x][y]<<";"<<endl;
    }

    
    
    
    vector<Attribute> targetAttributes;
    
    
    
    //add each attribute in the file
    
    for( std::string line; getline( metaDataInfo, line ); )
    {
        //...for each line in input
        vector<string> tokens; // Create vector to hold our words

    
        Tokenize(line, tokens, ":");
        Attribute attributeObj;
        
        attributeObj.name = tokens[0];
        attributeObj.cardinality = atoi(tokens[1].c_str());
        attributeObj.confirmations = 0;
        attributeObj.solutionQueryString = "";
        
        targetAttributes.push_back(attributeObj);
        
        cout<<"name = "<<attributeObj.name<<", card = "<<attributeObj.cardinality<<endl;
    }
    
    metaDataInfo.close();
    
    
    //for each query q_i, attach all other queries q_j satisfying Ct[q_i][q_j] = 0
    
    
    //vector<int> queriesSize;
    vector<Query> queries;
    
    //queriesSize.push_back(0);//query number 0 means non-existant query
    
    Query dummy;
    dummy.query_no = 0;
    dummy.result_size = 0;
    //dummy.queryList = NULL;
    //dummy.queryList = Query();
    
    queries.push_back(dummy);
    
    for(int i = 1;i < maxNoQueries+1;i++)
    {
        
        
        Query qi;
        
        qi.query_no = i;
        qi.result_size = Ct[i][i];
        
        //qi.queryList = new vector<Query>();
        
        qi.queryList.push_back(qi);
        
        for(int j = 1;j < maxNoQueries+1;j++)
        {
            
            if(Ct[i][j] == 0 && (Ct[j][j]+Ct[i][i] <= number_of_records))
            {
               Query qj;
               qj.query_no = j;
               qj.result_size = Ct[j][j];
               //qj.queryList = NULL;
               //qj.queryList = Query();
               qi.queryList.push_back(qj);  
            }
            
            
            
            
            
            
            
            
            
        
        }
            
        //queriesSize.push_back(qi.queryList->size());
        
        queries.push_back(qi);
        
        
    }
    
    
    
    //sort the queries according to their result sizes
   
    std::sort (queries.begin(), queries.end());
    
    
    // print out content:
    std::cout << "Queries vector contains:";
    for (int i = 0;i < maxNoQueries+1;i++)
      std::cout << ' ' << queries[i];
    std::cout << '\n';
    
    
    
    
   
    
    
    //queries is L in Algorithm 1
    //transform each queries[i] into a graph
    //find maximal independent sets summing up to number_of_records-queries[i].head.size
    
    clock_t begin = clock();
    
    
    
    
    vector<Query> intersectionSet[maxNoQueries+1];
    
    
    //vector<Query> indSet[maxNoQueries+1];
    
    bool positive[maxNoQueries+1];// = {false};
    
    //bool indSetPositive[maxNoQueries+1];// = {false};
    
    
    
    bool decreasing;
    int loopNo = 0;
    int solutionNo = 0;
    int iPositive = 0;
    
    vector<IncompleteSolution> incompleteSols;
    
    
    
    do
    {
      
      loopNo++;
      decreasing = false;
    
      for(int i = 1;i <= maxNoQueries;i++)
      {
        positive[i] = false;
        //indSetPositive[i] = false;
        intersectionSet[i].clear();
      }
      
      //indSetCount = 0;
      //iPositive = 0;
    
      //#pragma omp parallel for
      for(int i = begin_index;i <= end_index;i++)
      {
        
        if(queries[i].queryList.size() == 0)
            continue;
        
        
        
        vector<Query> iQueryList = queries[i].queryList;
    
   
        cout<<"Testing i = "<<i<<", size = "<<iQueryList.size()<<endl;
        

        
        intersectionSet[i] = iQueryList;
 
        
            
        for(int j = 1;j <= maxNoQueries;j++)
        {
            
            
            if(i == j)
                continue;

    
            vector<Query> jQueryList;
            
            
            jQueryList = queries[j].queryList;
      
            
            if(jQueryList.size() == 0)
               continue;
        
                
            
            //queryExistBS1   
            sort(intersectionSet[i].begin(),intersectionSet[i].end(),CompQueryNo()); //sorting according to query no since query result size is repetitive
           
            if(queryExistBS1(jQueryList[0],intersectionSet[i],0,intersectionSet[i].size()-1)!=-1)//isQueryExist(jQueryList[0],intersectionSet[i]))//isQueryExist(intersectionSet[i][0],jQueryList)) 
            {
               
                  vector<Query> intersec;
                  int intersec_sum = 0;

                  if(intersectionSet[i].size() <= jQueryList.size())
                     intersec = efficientIntersection(intersectionSet[i],jQueryList);//intersection(intersectionSet[i],jQueryList);
                  else
                     intersec = efficientIntersection(jQueryList,intersectionSet[i]);//intersection(intersectionSet[i],jQueryList);
                      
                  intersec_sum = sumResultSizes(intersec,0,intersec.size()-1);
                  
                  //display(intersec);
                  //cout<<"i = "<<i<<", j = "<<j<<": intersec sum = "<<intersec_sum<<endl;
                  
                  if(intersec_sum >= number_of_records)
                          intersectionSet[i] = intersec;
                  
            
            }
           
            
            
            
            
            
            
            
            
            
            
            
            
        }
        

        
        int sum = sumResultSizes(intersectionSet[i],0,intersectionSet[i].size()-1);
        
        if((sum == number_of_records) && testZeroCo_Occurrence(intersectionSet[i]) && isValidCardinality(targetAttributes,intersectionSet[i].size()) != -1)
        {
            
               positive[i] = true;
          
               //cout<<"loop No = "<<loopNo<<", i = "<<i<<endl;
               iPositive++;
               
               
            
        }
        else
            positive[i] = false;
  
               
        
 
        
        
        
    }

    //cout<<endl;
   
  
    

    vector<Query> solutionMetaInfo;

    
    for(int i = begin_index;i <= end_index;i++)
    {
       if(positive[i])
       {
          
      
               
                 sort(intersectionSet[i].begin(),intersectionSet[i].end(),CompQueryNo()); //sorting according to query no since query result size is repetitive
                 
                 bool exist = false;
                 
                 for(int k = 0;k < solutionMetaInfo.size();k++)
                   if(solutionMetaInfo[k].query_no==intersectionSet[i].size() && (toString(solutionMetaInfo[k].queryList) == toString(intersectionSet[i])))
                   {
                       //confirmations[i]++;
                       solutionMetaInfo[k].result_size++;
                       exist = true;
                       break;
                   }
                 
                 if(!exist)
                 {
                     Query q;
                     q.query_no = intersectionSet[i].size(); //card
                     q.result_size = 1; //confirmations
                     q.queryList = intersectionSet[i]; //sorted according to query no
                     solutionMetaInfo.push_back(q);
                 }
                 
        }    
   
       
      
    
    }
  
  
    //detect solutions with shared queries and ignore them in the current loop
  
    for(int i = 0; i < solutionMetaInfo.size();i++)
    {
            bool repetitionFlag = false; 
               
            for(int j = i+1; j < solutionMetaInfo.size();j++)
            {
             
                      vector<Query> S;
                      
                      
                      if(solutionMetaInfo[i].queryList.size() <= solutionMetaInfo[j].queryList.size())
                         S = efficientIntersection(solutionMetaInfo[i].queryList,solutionMetaInfo[j].queryList);
                      else
                         S = efficientIntersection(solutionMetaInfo[j].queryList,solutionMetaInfo[i].queryList);
                          
                      
                      //cout<<" i = "<<i<<" j = "<<j<<" size = "<<S.size()<<endl;
                              
                      
                      if(S.size())
                      {
                          //if(solutionMetaInfo[i].result_size < 30 || solutionMetaInfo[i].result_size != solutionMetaInfo[i].queryList.size())
                          solutionMetaInfo[i].tag = -1;
                          //if(solutionMetaInfo[i].result_size < 30 || solutionMetaInfo[j].result_size != solutionMetaInfo[j].queryList.size())
                          solutionMetaInfo[j].tag = -1;
                          repetitionFlag = true;
                          break;
                      }
                  
            }
               
    }
    
    
    
  
    //detect solutions with shared queries and ignore them in the current loop
  


  
    vector<Query> recoveredQueries;

    if(solutionMetaInfo.size())
    {
      sort(solutionMetaInfo.begin(),solutionMetaInfo.end(),CompResultSize());
    
      for(int k = 0; k < solutionMetaInfo.size();k++)
      {
          
          if(solutionMetaInfo[k].tag != -1 && isClose(solutionMetaInfo[k].result_size,solutionMetaInfo[k].queryList.size()))
          {
            solutionNo++;
            cout<<"Solution # "<<solutionNo<<", card = "<<solutionMetaInfo[k].query_no<<", confirmations = "<<solutionMetaInfo[k].result_size<<endl;
            cout<<toString(solutionMetaInfo[k].queryList)<<endl;
            cout<<"------------------------------------------------------------------------------"<<endl;
            
            
            outMetaFile<<"Solution # "<<solutionNo<<", card = "<<solutionMetaInfo[k].query_no<<", confirmations = "<<solutionMetaInfo[k].result_size<<endl;
            outMetaFile<<toString(solutionMetaInfo[k].queryList)<<endl;
            outMetaFile<<"------------------------------------------------------------------------------"<<endl;
            
            for(int m = 0;m < solutionMetaInfo[k].queryList.size();m++)
                recoveredQueries.push_back(solutionMetaInfo[k].queryList[m]);
          }
          else if(isClose(solutionMetaInfo[k].result_size,solutionMetaInfo[k].queryList.size()) && isValidCardinality(targetAttributes,solutionMetaInfo[k].query_no) != -1)
          {
              
              
            cout<<"Solution that might have false positives, card = "<<solutionMetaInfo[k].query_no<<", confirmations = "<<solutionMetaInfo[k].result_size<<endl;
            cout<<toString(solutionMetaInfo[k].queryList)<<endl;
            cout<<"------------------------------------------------------------------------------"<<endl;
            
            
            outMetaFile<<"Solution that might have false positives, card = "<<solutionMetaInfo[k].query_no<<", confirmations = "<<solutionMetaInfo[k].result_size<<endl;
            outMetaFile<<toString(solutionMetaInfo[k].queryList)<<endl;
            outMetaFile<<"------------------------------------------------------------------------------"<<endl;
            
          }
          else if(isValidCardinality(targetAttributes,solutionMetaInfo[k].query_no) != -1 && solutionMetaInfo[k].result_size > 1)
          {


               
            cout<<"Solution with few confirmations that might be valid with low prob, card = "<<solutionMetaInfo[k].query_no<<", confirmations = "<<solutionMetaInfo[k].result_size<<endl;
            cout<<toString(solutionMetaInfo[k].queryList)<<endl;
            cout<<"------------------------------------------------------------------------------"<<endl;
            
            
            outMetaFile<<"Solution with few confirmations that might be valid with low prob, card = "<<solutionMetaInfo[k].query_no<<", confirmations = "<<solutionMetaInfo[k].result_size<<endl;
            outMetaFile<<toString(solutionMetaInfo[k].queryList)<<endl;
            outMetaFile<<"------------------------------------------------------------------------------"<<endl;
          }
          
      }
    }
  
  
  
   if(recoveredQueries.size() > 0)
   {
       decreasing = true;
      
  
      
      sort(recoveredQueries.begin(),recoveredQueries.end(),CompQueryNo());
      cout<<"recovered Queries size = "<<recoveredQueries.size()<<endl;
      cout<<toString(recoveredQueries)<<endl;
       
      for(int i = 1;i <= maxNoQueries;i++)
      {
          
          if(queries[i].queryList.size() == 0)
              continue;
      
          
          vector<Query> iQueryListNew;
          
                
          if(queryExistBS1(queries[i].queryList[0],recoveredQueries,0,recoveredQueries.size()-1) != -1)
          {
              //queries[i].queryList = iQueryListNew;
              queries[i].queryList.clear();
              
              //cout<<"size becomes = "<<queries[i].queryList.size()<<endl;
              continue;
          }
          else
              iQueryListNew.push_back(queries[i].queryList[0]);
              
              
          for(int j = 1;j < queries[i].queryList.size();j++)
          {
              
             if(queryExistBS1(queries[i].queryList[j],recoveredQueries,0,recoveredQueries.size()-1) == -1)//!checkQueryExist(queries[i].queryList[j],recoveredQueries))
                  iQueryListNew.push_back(queries[i].queryList[j]);
          }
          
          //cout<<"size before and after: "<<queries[i].queryList.size()<<" \t";
          
          queries[i].queryList.clear();
          queries[i].queryList = iQueryListNew;
          
          //cout<<queries[i].queryList.size()<<endl;
      }
      
      //sort the queries according to their result sizes
   
      std::sort (queries.begin(), queries.end(),CompQueryListSize());
      
      
   }
    
    
    
    cout<<"recovered Queries size = "<<recoveredQueries.size()<<endl;
    cout<<"iPositive = "<<iPositive<<endl;
  
    //cout<<"indSetCount = "<<indSetCount<<endl;
    cout<<"Finished loop No = "<<loopNo<<endl;
   
    cout<<"###########################################################################################"<<endl;
    
}while(decreasing);
    
    
   //outMetaFile.close();
    
   //Executed when Hours-per-week and AGE are not considered 
   clock_t end = clock();
   double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
   
   
  
  
    
   cout<<"Total time in mins = "<< elapsed_secs/60 <<endl;
    
  
   //outfile<<"Total time in mins = "<< elapsed_secs/60 <<endl;
   
   //outfile.close();
   
   for(int i = 0; i < maxNoQueries+1; ++i) 
      delete [] Ct[i];

   delete [] Ct;
    
    
   return 0;

}

