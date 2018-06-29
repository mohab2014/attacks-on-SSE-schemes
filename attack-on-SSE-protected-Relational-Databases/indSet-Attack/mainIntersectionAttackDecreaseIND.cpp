


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


class Attribute
{
public:
    string name;
    int cardinality;
    //int attempts_to_resolve;
    string solutionQueryString;
    int confirmations;
};



 



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
        
        //cout<<"degree = "<<degree<<endl;
        
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

bool isValidCardinality(vector<Attribute> a, int card)
{
    for(int i = 0;i < a.size();i++)
        if(a[i].cardinality == card)
            return true;
    return false;
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



//-------------------------------------------------------------



/*
 * 
 */
int main(int argc, char** argv) 
{ 


    

    int x, y, f; //read the three integers, queries number x and y and their joint frequency value f 
    char c1, c2; //read two commas


           
    
    
    
    
    if(argc != 7)
    {

       std::cout<<"./attack begin-query-no end-query-no maxNoQueries observedMatrixFile no_records ResultFileName\n"; //meta_data_info\n";
       exit(1);
    }

    
    int begin_index = atoi(argv[1]);
    
    int end_index = atoi(argv[2]);
    
    maxNoQueries = atoi(argv[3]);
    
    std::ifstream infile(argv[4]);
    
    number_of_records = atoi(argv[5]);
    
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
    
    
    
    double sum = 0;
    
    for(int i = begin_index;i <= end_index;i++)
    {
        
        sum += queries[i].queryList.size();
           
    }
   
    double mean = (double)sum/end_index;
    
    
    
    for(int i = begin_index;i <= end_index;i++)
    {
        
        sum += (queries[i].queryList.size()-mean)*(queries[i].queryList.size()-mean);
           
    }
   
    
    double var = sum/end_index;
    
    cout<<"mean = "<<mean<<", var = "<<var<<endl;
    
    
    //queries is L in Algorithm 1
    //transform each queries[i] into a graph
    //find maximal independent sets summing up to number_of_records-queries[i].head.size
    
    clock_t begin = clock();
    
    
    
    
    vector<Query> intersectionSet[maxNoQueries+1];
    
    
    vector<Query> indSet[maxNoQueries+1];
    
    bool positive[maxNoQueries+1];// = {false};
    
    bool indSetPositive[maxNoQueries+1];// = {false};
    
    
    
    bool decreasing;
    int loopNo = 0;
    int solutionNo = 0;
    int indSetCount = 0;
    int iPositive = 0;
    
    std::ofstream outMetaFile(argv[6]);
    
    do
    {
      
      loopNo++;
      decreasing = false;
    
      for(int i = 1;i <= maxNoQueries;i++)
      {
        positive[i] = false;
        indSetPositive[i] = false;
        intersectionSet[i].clear();
      }
      
      indSetCount = 0;
      iPositive = 0;
    
      //#pragma omp parallel for
      for(int i = begin_index;i <= end_index;i++)
      {
        
        if(queries[i].queryList.size() == 0)
            continue;
        
        
        
        vector<Query> iQueryList = queries[i].queryList;
    
   
        cout<<"Testing i = "<<i<<endl;
        
        //cout<<"*";
        
        intersectionSet[i] = iQueryList;
        
        indSet[i] =  independentSetGreedyMax(iQueryList, number_of_records); //independentSetGreedyMin(iQueryList,maxNoQueries,number_of_records);
             
        sort(indSet[i].begin(),indSet[i].end(),CompQueryNo()); 
        
       
        int sumIndSet = sumResultSizes(indSet[i],0,indSet[i].size()-1);
        
        Query maxDegreeQuery = maximumDegreeQuery(iQueryList);
        
        
        if(sumIndSet  == number_of_records)
        {
          cout<<"loop = "<<loopNo<<", indSet: i= "<<i<<", sum = "<<sumIndSet<<", size = "<<indSet[i].size()<<", head = "<<iQueryList[0].query_no<<", maxDegree = "<<maxDegreeQuery.tag<<endl;
          //", size of a max ind set is at least = "<<(double)((iQueryList.size()-1)/(maxDegreeQuery.tag+1))<<endl;
          
          sort(indSet[i].begin(),indSet[i].end(),CompQueryNo());
          for (int k = 0; k < indSet[i].size(); k++)
                     cout<<indSet[i][k].query_no<<":"<<indSet[i][k].result_size<<";";
          cout<<endl;
          indSetCount++;
          indSetPositive[i] = true;
        }
        /*else
        {
            cout<<"loop = "<<loopNo<<", indSet: i= "<<i<<", sum = "<<sumIndSet<<", size = "<<indSet.size()<<", head = "<<iQueryList[0].query_no<<", maxDegree = "<<maxDegreeQuery.tag<<endl;
        }*/
        
        
            
        
        

        
        
 
        
        
        
     }

     //cout<<endl;
   
  
    

    vector<Query> indSetSolution;

    
    for(int i = begin_index;i <= end_index;i++)
    {
       
       
       if(indSetPositive[i])
       {
                 bool exist = false;
                 
                 for(int k = 0;k < indSetSolution.size();k++)
                   if(indSetSolution[k].query_no==indSet[i].size() && (toString(indSetSolution[k].queryList) == toString(indSet[i])))
                   {
                       //confirmations[i]++;
                       indSetSolution[k].result_size++;
                       exist = true;
                       break;
                   }
                 
                 if(!exist)
                 {
                     Query q;
                     q.query_no = indSet[i].size(); //card
                     q.result_size = 1; //confirmations
                     q.queryList = indSet[i]; //sorted according to query no
                     indSetSolution.push_back(q);
                 }
       }
      
    
    }
  
  
    
    
    
  
    //detect solutions with shared queries and ignore them in the current loop
  
    for(int i = 0; i < indSetSolution.size();i++)
    {
            bool repetitionFlag = false; 
               
            for(int j = i+1; j < indSetSolution.size();j++)
            {
             
                      vector<Query> S;
                      
                      
                      if(indSetSolution[i].queryList.size() <= indSetSolution[j].queryList.size())
                         S = efficientIntersection(indSetSolution[i].queryList,indSetSolution[j].queryList);
                      else
                         S = efficientIntersection(indSetSolution[j].queryList,indSetSolution[i].queryList);
                          
                      
                      //cout<<" i = "<<i<<" j = "<<j<<" size = "<<S.size()<<endl;
                              
                      
                      if(S.size())
                      {
                          indSetSolution[i].tag = -1;
                          indSetSolution[j].tag = -1;
                          repetitionFlag = true;
                          break;
                      }
                  
            }
               
    }

    vector<Query> recoveredQueries;
   
    if(indSetSolution.size())
    {
      sort(indSetSolution.begin(),indSetSolution.end(),CompQueryNo());
    
      for(int k = 0; k < indSetSolution.size();k++)
      {
          
          if(indSetSolution[k].tag != -1 && isClose(indSetSolution[k].result_size,indSetSolution[k].queryList.size()))
          {
            solutionNo++;
            cout<<"Solution # "<<solutionNo<<", card = "<<indSetSolution[k].query_no<<", confirmations = "<<indSetSolution[k].result_size<<endl;
            cout<<toString(indSetSolution[k].queryList)<<endl;
            cout<<"------------------------------------------------------------------------------"<<endl;
            
            
            outMetaFile<<"Solution # "<<solutionNo<<", card = "<<indSetSolution[k].query_no<<", confirmations = "<<indSetSolution[k].result_size<<endl;
            outMetaFile<<toString(indSetSolution[k].queryList)<<endl;
            outMetaFile<<"------------------------------------------------------------------------------"<<endl;
            
            for(int m = 0;m < indSetSolution[k].queryList.size();m++)
                recoveredQueries.push_back(indSetSolution[k].queryList[m]);
          }
          else if(isClose(indSetSolution[k].result_size,indSetSolution[k].queryList.size()))
          {
              
              
            cout<<"Solution that might have false positives, card = "<<indSetSolution[k].query_no<<", confirmations = "<<indSetSolution[k].result_size<<endl;
            cout<<toString(indSetSolution[k].queryList)<<endl;
            cout<<"------------------------------------------------------------------------------"<<endl;
            
            
            outMetaFile<<"Solution that might have false positives, card = "<<indSetSolution[k].query_no<<", confirmations = "<<indSetSolution[k].result_size<<endl;
            outMetaFile<<toString(indSetSolution[k].queryList)<<endl;
            outMetaFile<<"------------------------------------------------------------------------------"<<endl;
            
          }
          
      }
        
    }

  

  
  
    //remove solutions with many confirmations
    //decreasing = true
  
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
      
    }
    
    
    //cout<<"iPositive = "<<iPositive<<endl;
  
    cout<<"indSetCount = "<<indSetCount<<endl;
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


