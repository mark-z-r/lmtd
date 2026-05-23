# LMTD

LMTD (pronounced 'limited' but with the vowels shortened) is a desktop
enviroment. Its goal is simple: low resource usage. 

It achieves this with seperating each feature into a seperate program.
on the surface, it may seem like this will make stuff take more memory and run slower.
but that is not the case. by keeping stuff seperate, each part can be optimized more.
'Do one thing really well'.


I came up with the name as a shortening of limited, but LMTD is not an
abbrevation, it is just a name.

## Choices
- Wayland
    - labwc (which is a display server *and* a compositor) only uses slightly
      more memory than Xorg ( which is only a display server )
    - I think I could make an even more lightweight compositor

## TODOs
- [ ] make a compositor
- [ ] make a background daemon 
    - [x] works (displays an image)
    - [ ] not jank 
