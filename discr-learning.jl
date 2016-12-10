using BayesNets
using LightGraphs
#using Graphs
using PGFPlots
using DataFrames
#using TikzGraphs
using Iterators

function k2!(g::DAG)
    
  bestscore=-Inf;
  shuffle!(arr1)
  shuffle!(arr2)

    for i in arr1
        #for j in arr2[1:maxparents]
          for j in arr2
            # note scoring neigbors are always retained
    
            LightGraphs.add_edge!(g,j,i);
          if (is_cyclic(g)) 
                LightGraphs.rem_edge!(g,j,i);   
          else
                newscore=bayesian_score(g, nodes, df) # score
            if (bestscore < newscore) 
               bestscore=newscore
            else
                    LightGraphs.rem_edge!(g,j,i);
            end
          end
    
       end
    end
    return bestscore
end

function climb(prevgraph::DAG, prevscore)

    bestscore=prevscore
    bestgraph=deepcopy(prevgraph)
    toygraph=deepcopy(bestgraph)
    println("Starting from:", bayesian_score(toygraph, nodes, df))
    
   for j in arr1   
        for i in arr2
            
            if ( LightGraphs.has_edge(toygraph,j,i) || LightGraphs.has_edge(toygraph,i,j) ) 
                LightGraphs.rem_edge!(toygraph,j,i)
                LightGraphs.rem_edge!(toygraph,i,j)
                newscore=bayesian_score(toygraph, nodes, df)
                
                if (bestscore < newscore) 
                    println("Better graph found:", newscore)
                    bestgraph=deepcopy(toygraph)
                    bestscore=newscore
                  
                end
                
                LightGraphs.add_edge!(toygraph,i,j)
                if ( !is_cyclic(toygraph))
                  newscore=bayesian_score(toygraph, nodes, df)
                  if (bestscore < newscore)
                    println("Better graph found:", newscore)
                    bestgraph=deepcopy(toygraph)
                    bestscore=newscore

                  end
                end
                LightGraphs.rem_edge!(toygraph,i,j)
                
                LightGraphs.add_edge!(toygraph,j,i)
                if ( !is_cyclic(toygraph))
                newscore=bayesian_score(toygraph, nodes, df)
                 if (bestscore < newscore)
                    println("Better graph found:", newscore)
                    bestgraph=deepcopy(toygraph)
                    bestscore=newscore

                 end
                end
                
            end
            toygraph=deepcopy(bestgraph)
            
            if ( !LightGraphs.has_edge(toygraph,j,i) && !LightGraphs.has_edge(toygraph,i,j) ) 
            
                LightGraphs.add_edge!(toygraph,j,i)
            
                 if ( !is_cyclic(toygraph))
                    newscore=bayesian_score(toygraph, nodes, df)
                    if (bestscore < newscore) 
                        
                        println("Better graph found:", newscore)
                        
                        bestgraph=deepcopy(toygraph)
                        bestscore=newscore
                        
                    end
                end
            end
            toygraph=deepcopy(bestgraph)
            
            if ( !LightGraphs.has_edge(toygraph,j,i) && !LightGraphs.has_edge(toygraph,i,j) ) 
            
                LightGraphs.add_edge!(toygraph,i,j)
            
                 if ( !is_cyclic(toygraph))
                    newscore=bayesian_score(toygraph, nodes, df)
                    if (bestscore < newscore) 
                        
                        println("Better graph found:", newscore)
                        bestgraph=deepcopy(toygraph)
                        bestscore=newscore
                       
                    end
                end
            end
            toygraph=deepcopy(bestgraph)
            
        end
        println("now on vertice:",j)
    end
    println("best score recorded in climb:", bayesian_score(bestgraph, nodes, df))
    return bestgraph
end

function output(graph::DAG, filename)
    f=open(filename,"w")
    for vertice in LightGraphs.vertices(graph)
        for neighbor in LightGraphs.out_neighbors(graph,vertice)
            @printf(f, "%s,%s\n", nodes[vertice],nodes[neighbor])
        end
    end
    close(f)
end


# Program start

println("Program starting")
infile="/Users/ianakushch/Documents/Stanford/CS238/output3.csv"
f = open(infile)
df = readtable(infile)
nodes=names(df)
close(f)

maxparents=7
arr1=[]
arr2=[]
for j = 1:ncol(df)
    push!(arr1,j)
    push!(arr2,j)
end

championScore=-Inf
championGraph=DAG()

for i in 1:10000
    
   g = DAG(ncol(df))
   score=k2!(g)

   if (score > championScore) 
       championGraph=deepcopy(g)
       championScore=score
   end

    if (i%1000)==0 
        println("iteration: ", i)
    end
end 

while true
    
  climbGraph=climb(championGraph, championScore)
  climbScore=bayesian_score(climbGraph, nodes, df)
    
  if climbScore >  championScore
     championScore = climbScore
     championGraph = deepcopy(climbGraph)
  else
     break
  end
end

output(climbGraph,"output1-ed-full.gph")





