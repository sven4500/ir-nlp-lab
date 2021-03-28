function y = compmet2(r,k)

    qmax = size(r,1);
    gmax = max(r(:));
    
    y=zeros(qmax,5);
    for i=1:qmax
        y(i,:) = compmet(r(i,1:end),k,gmax);
    end

end
