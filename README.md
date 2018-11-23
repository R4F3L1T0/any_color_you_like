# Any color you like
### A genetic algorithm that finds the correspondance among two sets of label that maximizes their overlap.

*Any color you like* (named after the Pink Floyd song in *The dark side of the Moon*), it's an optimization algorithm that finds the extremal of a given cost function employing genetic search, it's versatile and can be easily adapted to solve a great variety of problems.   
It has proved to be more efficient, fast and accurate than more modern approaches (i.e. clustering methods). Here you find it's implementation for a particular issue I faced during my Master Thesis in wich I used it to found the highest overlap among two model realization of the same DNA filament obtained as output of independent simulations.

Its functioning it's way easier to understand by making examples.  
In the picture below (subfigure a), you find two sets of objects, you can
immagine them to be two sets of binding domains of a polymer model or to be
two sets of earthquake signals and so on. Wherever you carry your 
imagination, the goal is to find the elements correspondance that maximize
the overlap (you can define your custom overlap as appropriate).  
In this particular case you can see you the action of *any color you like*
rearrange the binding domains and finds the best counterpart for each one.  
![example_of_result](https://user-images.githubusercontent.com/44372889/48962049-61e99700-ef2f-11e8-8e99-58bf4aa75e56.png)
