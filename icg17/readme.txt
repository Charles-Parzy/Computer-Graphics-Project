1. Perspective Projection

To implement the perspective projection we needed to create the projection matrix in the screenshot perspective.png. To achieve that we used some trigonometric rules (see  screenshot Camera_scheme.png). On this screenshot, we can see that the tangent of the angle ϴ/2 is equal to t/n, where ϴ is the field of view, t is the half of the window height and n is near distance.
So, the inverse of tan(ϴ/2) is n/t. Furthermore, as the aspect is the ratio between the width and the height, then n/t divided by the aspect is equal to n/t * h/w. As t = h/2 and r = w/2, then n/t divided by the aspect is : n/(h/2) * h/w = n/(w/2) = n/r, which gives us the first coefficient of the projection matrix. Moreover, we have now all the values needed to implement this matrix.

2. Trackball

In the ProjectOntoSurface function, we dissociate two cases: whenever the position of the mouse cursor is inside or outside of the trackball. To know that, we computed the squared norm of the vector representing the point, and we compared it with the squared of the trackball’s radius. If it’s lower than the square of the radius, the point is inside the surface, otherwise it’s outside.
Depending of the cases, we apply the given formulas form the statement.
In the Drag function, since 
a . b = |a| |b| cos(ϴ), where ϴ is the angle between a and b, and . is the dot product,
the norms of current_pos and anchor_pos_ is 1 as the radius is 1
so arcos(current_pos . anchor_pos_) = arcos(cos(ϴ)) = ϴ.
Actually, we compute the arcos of the minimum of 1 and the dot product because we are working with floating points.
The rotation’s direction (rotation_axis) is the cross product between current_pos and anchor_pos_.
After that, we just construct the matrix.
Finally, in the main, we updated the trackball_matrix by multiplying the old one by the new rotation matrix.

For the Zooming, we keep the old y position each time we press the mouse button. We translate by the difference between the y position and the old y position, times a constant factor.

3. Triange Grid and Animation

Creating the grid:
To generate the triangle grid we initially had to generate a grid of vertices. The first position would be at (-1, 1) and the last at (1, -1) where all columns in a row are indexed before moving on to the next row. After generating these positions and putting them into the vertices vector, the indices for for the mesh were generated. After initially attempting to a create a strip of triangles and failing to do so, we decided to fall back to indexing all three vertex positions before moving on to the next triangle. Thus we had to change GL_TRIANGLE_STRIP to GL_TRIANGLES in the glDrawElements(…) call.

The main difficulty with creating the grid was wrapping our head around the order in which indices had to be added to the vector for all triangles to be generated correctly. 

Making the grid animated:
We knew that the height had to be based off of the sine function. It also had to be shifted over each time we moved down a row. Thus we came up with this formula as our initial draft:
0.1 * sin(10 * (uv[0] + uv[1]) + (time * 2)) - 0.1
Where 0.1 was the height of the wave, 10 defined the distance between each crest and 2 defined the speed of the wave. While this formula did what was required we didn’t really understand why it worked. Later, while working on the bonus section, we discovered that our formula matched the height element of the Gerstner wave function(see gerstner_wave.png). Thus we rewrote the calculation in a cleaner way:
a * sin(w * (x,y) . (d1,d2) + (time * o)) - a
Where a is the amplitude of the wave, w is wavelength, o is the speed, and (d1, d2) is the direction vector.

The difficult part of generating the wave was debugging the wrong solutions since you could not print to console from the shader file. Thus we used color as a debugging tool and changed the color of the wave in the fshader depending on values that we were unsure of(see debug.png for an example).

4. Water animation (Bonus)
To see the water animation please change the ‘water’ variable to true vshader file. To see the wave behaviour better you may also want to flip debug to true in the fshader file. To make the initial sine wave look like water we used the Gerstner wave function(again, see gerstner_wave.png). We first initialized the attributes for 5 wave functions, 3 of them circular and 2 directional and the caculated the height and new x and y values. The only difference between the circular and directional waves apart from constant values was that the direction vectors for the circular waves depended on the x and y values. The difference between the Gerstner wave and the normal sine wave is that not only does the height value changes, but also the x and y values do too. This gives an illusion of water particles riding the wave and not just hovering up and down. The combination of multiple waves also adds to the immersion. 

The formulas for new x and y are almost identical to the height formula except for few key differences:
a * q * d * cos(w * (x,y) . (d1,d2) + (time * o))
Where the new variable q is the roundness of the wave and is in range (0,1) though it is advised for it to be in the lower range to not add “loops” at the top of your waves.

Another change that we made from just a basic wave is that we changed the sin(…) in the height formula to 2 * ((sin(…) + a)/2)^1.5. This change made the sides of each bump in the wave steeper.

The main difficulty with this task was understanding the Gerstner wave function. Once we understood it, implementing it was choresome and required much debugging using the colors, but was rewarding at the end.