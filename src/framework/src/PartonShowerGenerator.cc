// -----------------------------------------
// JetScape (modular/task) based framework
// Intial Design: Joern Putschke (2017)
//                (Wayne State University)
// -----------------------------------------
// License and Doxygen-like Documentation to be added ...


#include "PartonShowerGenerator.h"
#include "PartonShower.h"
#include "JetEnergyLoss.h"
#include "JetScapeLogger.h"

#include<iostream>

using namespace std;

void PartonShowerGenerator::DoShower(JetEnergyLoss &j)
{
  double tStart=0;
  double currentTime=0;

  VERBOSESHOWER(8)<<"Hard Parton from Initial Hard Process ...";
  VERBOSEPARTON(6,*j.GetShowerInitiatingParton());

  // consider pointers for speed up ... 
  vector<Parton> pIn; vector<Parton> pOut;
  vector<Parton> pInTemp; vector<Parton> pOutTemp;
  vector<Parton> pInTempModule;
  
  vector<node> vStartVec; vector<node> vStartVecOut;
  vector<node> vStartVecTemp;
  
  node vStart;
  node vEnd;
  
  pIn.push_back(*j.GetShowerInitiatingParton());

  // Add here the Hard Shower emitting parton ...
  vStart=j.GetShower()->new_vertex(make_shared<VertexBase>());
  vEnd=j.GetShower()->new_vertex(make_shared<VertexBase>());
  j.GetShower()->new_parton(vStart,vEnd,make_shared<Parton>(*j.GetShowerInitiatingParton()));

  // start then the recursive shower ...
  vStartVec.push_back(vEnd);
  //vStartVecTemp.push_back(vEnd);
  
  // ISSUE: Probably not yet 100% wrt to time step evolution ...
  // Logic mistake to remove the original ones when no split occured !!??? Follow up!!!!
  REMARK<<"DoShower() Splitting including time evolution (allowing non-splits at later times) implemeted correctly (should be made nicer/pointers). To be checked!!!";

  // --------------------------------------------
  
  do
    {
      VERBOSESHOWER(7)<<"Current time = "<<currentTime<<" with #Input "<<pIn.size();     
      currentTime += j.GetDeltaT();

      // --------------------------------------------
      
      for (int i=0;i<pIn.size();i++)
	{
	  //DEBUG:
	  //cout<<currentTime<<" pIn size = "<<pIn.size()<<" "<<i<<" "<<pIn[i].pt()<<endl;

	  pInTemp.push_back(pIn[i]);
	  pInTempModule.push_back(pIn[i]);
	  
	  j.SentInPartons(currentTime,pIn[i].pt(),pInTempModule,pOutTemp);

	  vStart=vStartVec[i];
	  vStartVecTemp.push_back(vStart);

	  //DEBUG:
	  //cout<<vStart<<endl;
	  // --------------------------------------------
	  for (int k=0;k<pOutTemp.size();k++)
	    {
	      vEnd=j.GetShower()->new_vertex(make_shared<VertexBase>(0,0,0,currentTime));	    	      
	      j.GetShower()->new_parton(vStart,vEnd,make_shared<Parton>(pOutTemp[k]));	     
	      	     
	      //DEBUG:
	      //cout<<vStart<<"-->"<<vEnd<<endl;
	      //cout<<pOutTemp[k];
	      //cout<<vStartVec.size()<<endl;
	      //cout<<pInTempModule.size()<<endl;
	      
	      vStartVecOut.push_back(vEnd);
	      pOut.push_back(pOutTemp[k]);

	      // --------------------------------------------
	      // Add new roots from ElossModules ...
	      // (maybe add for clarity a new vector in the signal!???)
	      // Otherwise keep track of input size (so far always 1
	      // and check if size > 1 and create additional root nodes to that vertex ...
	      // Simple Test here below:
	      // DEBUG:
	      //cout<<"In JetEnergyloss : "<<pInTempModule.size()<<endl;
	      
	      if (pInTempModule.size()>1)
		{
		  VERBOSESHOWER(7)<<pInTempModule.size()-1<<" new root node(s) to be added ...";
		  //cout<<pInTempModule.size()-1<<" new root node(s) to be added ..."<<endl;
		  
		  for (int l=1;l<pInTempModule.size();l++)
		    {
		      node vNewRootNode=j.GetShower()->new_vertex(make_shared<VertexBase>(0,0,0,currentTime-j.GetDeltaT()));
		      j.GetShower()->new_parton(vNewRootNode,vEnd,make_shared<Parton>(pInTempModule[l]));
		    }
		}
	      // --------------------------------------------
	      
	      if (k==0)
		{
		  pInTemp.pop_back();       		  
		  vStartVecTemp.pop_back();		 
		}	  
	    }
	  // --------------------------------------------
	 
	  pOutTemp.clear();
	  pInTempModule.clear();
	}

      // --------------------------------------------
      
      pIn.clear();
      
      pIn.insert(pIn.end(),pInTemp.begin(),pInTemp.end());
      pIn.insert(pIn.end(),pOut.begin(),pOut.end());
      
      pOut.clear();
      pInTemp.clear();
          
      vStartVec.clear();
      
      vStartVec.insert(vStartVec.end(),vStartVecTemp.begin(),vStartVecTemp.end());
      vStartVec.insert(vStartVec.end(),vStartVecOut.begin(),vStartVecOut.end());
           
      vStartVecOut.clear();
      vStartVecTemp.clear();
    }
  while (currentTime<j.GetMaxT()); //other criteria (how to include; TBD)

  // --------------------------------------------
  
  // real ceck using mom. consveration at vertex ...!!!!
  //pShower->save(&(cout<<BOLDCYAN));
  
  pIn.clear();pOut.clear();pInTemp.clear();pOutTemp.clear();
  vStartVec.clear();
  
}