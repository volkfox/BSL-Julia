# beginning discretization program: three algorithms: 7,9,bayesian

using DataFrames
using DataArrays
infile="/Users/ianakushch/Documents/Stanford/CS238/bayes-minprev.csv"
f = open(infile)
df = readtable(infile)
nodes=names(df)
close(f)

outf1 = DataFrame()
outf2 = DataFrame()
outf3 = DataFrame()
politydisc = CategoricalDiscretizer(df[:polity])

counts = get_discretization_counts(politydisc, df[:polity])
println("polity counts:", counts)

outf1[:polity]= encode(politydisc, df[:polity])
outf2[:polity]= encode(politydisc, df[:polity])
outf3[:polity]= encode(politydisc, df[:polity])

discalgs = [("Uniform Width7 ", DiscretizeUniformWidth(7)),
                ("Uniform Width9", DiscretizeUniformWidth(9)),
                            ("Bayesian Blocks", DiscretizeBayesianBlocks())]

for var in nodes[5:end]
    
    disc = LinearDiscretizer(binedges(discalgs[1][2], df[var]))
    counts = get_discretization_counts(disc, df[var])
    println("variable ",var, " name:", discalgs[1][1] , " counts:", counts)
    outf1[var]= encode(disc, df[var])
    
    disc = LinearDiscretizer(binedges(discalgs[2][2], df[var]))
    counts = get_discretization_counts(disc, df[var])
    println("variable ",var, " name:", discalgs[2][1] , " counts:", counts)
    outf2[var]= encode(disc, df[var])
    
    disc = LinearDiscretizer(binedges(discalgs[3][2], df[var]))
    counts = get_discretization_counts(disc, df[var])
    println("variable ",var, " name:", discalgs[3][1] , " counts:", counts)
    outf3[var]= encode(disc, df[var])
    
end

writetable("/Users/ianakushch/Documents/Stanford/CS238/output1.csv", outf1)
writetable("/Users/ianakushch/Documents/Stanford/CS238/output2.csv", outf1)
writetable("/Users/ianakushch/Documents/Stanford/CS238/output3.csv", outf1)


