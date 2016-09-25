from nelder_mead import minimize
def rosen(x):
    return (1-x[0])**2+100*(x[1]-x[0]*x[0])**2
res = minimize(rosen, [420,-420])
print(res, rosen(res))
