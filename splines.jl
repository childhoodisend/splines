using Images, FileIO, TestImages, Random

function image_cut(img)
    ret = copy(img)
    for i in 1:size(ret)[1]
        for j in 1:size(ret)[2]
            if i % 2 == 0 || j % 2 == 0
                ret[i,j] = RGB(0,0,0)
            end
        end
    end
    return ret
end


function linear_spline_interpolate(img, parity)
    ret = copy(img)
    for i in 1:size(ret)[1]
        for j in 1:size(ret)[2]
            if j % 2 == parity
                if j == 1
                    ret[i,j] = ret[i,j+1]
                elseif j == size(ret)[2]
                    ret[i,j] = ret[i,j-1]
                else
                    ret[i,j] = 0.5*ret[i,j+1] + 0.5*ret[i,j-1]
                end
            end
        end
    end
    for i in 1:size(ret)[1]
        for j in 1:size(ret)[2]
            if i % 2 == parity
                if i == 1
                    ret[i,j] = ret[i+1,j]
                elseif i == size(ret)[1]
                    ret[i,j] = ret[i-1,j]
                else
                    ret[i,j] = 0.5*ret[i+1,j] + 0.5*ret[i-1,j]
                end
            end
        end
    end
    return ret
end

function trigonometric_spline_interpolate(img, parity)
    h = 2
    t = 0.5
    ret = copy(img)
    for i in 1:size(ret)[1]
        for j in 1:size(ret)[2]
            if j % 2 == parity
                if j == 1
                    ret[i,j] = ret[i,j+1]
                elseif j == size(ret)[2]
                    ret[i,j] = ret[i,j-1]
                else
                    ret[i,j] = RGB(min(test[i,j-1].r*(sin(h-t*h)/sin(h)) + test[i,j+1].r*(cos(t*h)/sin(h)), 1),
                    min(test[i,j-1].g*(sin(h-t*h)/sin(h)) + test[i,j+1].g*(cos(t*h)/sin(h)), 1),
                    min(test[i,j-1].b*(sin(h-t*h)/sin(h)) + test[i,j+1].b*(cos(t*h)/sin(h)), 1))
                end
            end
        end
    end
    for i in 1:size(ret)[1]
        for j in 1:size(ret)[2]
            if i % 2 == parity
                if i == 1
                    ret[i,j] = ret[i+1,j]
                elseif i == size(ret)[1]
                    ret[i,j] = ret[i-1,j]
                else
                    ret[i,j] = RGB(min(test[i-1,j].r*(sin(h-t*h)/sin(h)) + test[i+1,j].r*(cos(t*h)/sin(h)), 1),
                    min(test[i-1,j].g*(sin(h-t*h)/sin(h)) + test[i+1,j].g*(cos(t*h)/sin(h)), 1),
                    min(test[i-1,j].b*(sin(h-t*h)/sin(h)) + test[i+1,j].b*(cos(t*h)/sin(h)), 1))
                end
            end
        end
    end
    return ret
end

function cubic_spline_interpolate(img, parity)
    ret = copy(img)
    for i in 1:size(ret)[1]
        for j in 1:size(ret)[2]
            if j % 2 == parity
                if (rand((0,1))) == 0
                    if j == 1
                        ret[i,j] = ret[i,j+1]
                    elseif j == size(ret)[2]
                        ret[i,j] = ret[i,j-1]
                    elseif j+3 > size(ret)[2]
                        ret[i,j] = 0.5*ret[i,j+1] + 0.5*ret[i,j-1]
                    else
                        ret[i,j] = RGB(min(abs(0.375*ret[i,j-1].r + 0.75*ret[i,j+1].r - 0.125*ret[i,j+3].r),1),
                        min(abs(0.375*ret[i,j-1].g + 0.75*ret[i,j+1].g - 0.125*ret[i,j+3].g),1),
                        min(abs(0.375*ret[i,j-1].b + 0.75*ret[i,j+1].b - 0.125*ret[i,j+3].b),1))
                    end
                else
                    if j == 1
                        ret[i,j] = ret[i,j+1]
                    elseif j == size(ret)[2]
                        ret[i,j] = ret[i,j-1]
                    elseif j-3 < 1
                        ret[i,j] = 0.5*ret[i,j+1] + 0.5*ret[i,j-1]
                    else
                        ret[i,j] = RGB(min(abs(0.375*ret[i,j+1].r + 0.75*ret[i,j-1].r - 0.125*ret[i,j-3].r),1),
                        min(abs(0.375*ret[i,j+1].g + 0.75*ret[i,j-1].g - 0.125*ret[i,j-3].g),1),
                        min(abs(0.375*ret[i,j+1].b + 0.75*ret[i,j-1].b - 0.125*ret[i,j-3].b),1))
                    end
                end
            end
        end
    end
    for i in 1:size(ret)[1]
        for j in 1:size(ret)[2]
            if i % 2 == parity
                if (rand((0,1))) == 0
                    if i == 1
                        ret[i,j] = ret[i+1,j]
                    elseif i == size(ret)[1]
                        ret[i,j] = ret[i-1,j]
                    elseif i+3 > size(ret)[1]
                        ret[i,j] = 0.5*ret[i+1,j] + 0.5*ret[i-1,j]
                    else
                        ret[i,j] = RGB(min(abs(0.375*ret[i-1,j].r + 0.75*ret[i+1,j].r - 0.125*ret[i+3,j].r),1),
                        min(abs(0.375*ret[i-1,j].g + 0.75*ret[i+1,j].g - 0.125*ret[i+3,j].g),1),
                        min(abs(0.375*ret[i-1,j].b + 0.75*ret[i+1,j].b - 0.125*ret[i+3,j].b),1))
                    end
                else
                    if i == 1
                        ret[i,j] = ret[i+1,j]
                    elseif i == size(ret)[1]
                        ret[i,j] = ret[i-1,j]
                    elseif i-3 < 1
                        ret[i,j] = 0.5*ret[i+1,j] + 0.5*ret[i-1,j]
                    else
                        ret[i,j] = RGB(min(abs(0.375*ret[i+1,j].r + 0.75*ret[i-1,j].r - 0.125*ret[i-3,j].r),1),
                        min(abs(0.375*ret[i+1,j].g + 0.75*ret[i-1,j].g - 0.125*ret[i-3,j].g),1),
                        min(abs(0.375*ret[i+1,j].b + 0.75*ret[i-1,j].b - 0.125*ret[i-3,j].b),1))
                    end
                end
            end
        end
    end
    return ret
end


img = load("Workspace/conc/img.JPG")
cut = image_cut(img)
#linear = linear_spline_interpolate(cut,0)
#trigo = trigonometric_spline_interpolate(cut,0)
#cub = cubic_spline_interpolate(cut,0)
#save("cub.jpg",cub)
