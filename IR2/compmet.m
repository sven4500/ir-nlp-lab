% r - вектор оценок ранжированного списка
% k - уровень метрик
% gmax - максимально возможная оценка
function y = compmet(r,k,gmax)

    p=@(r,k,gmax)sum(r(1:k)>gmax./2)./length(r);
    
    cg=@(r,k)sum(r(1:k));
    dcg=@(r,k)sum(r(1:k)./log2((1:k)+1));
    idcg=@(r,k)sum(sort(r(1:k),'descend')./log2((1:k)+1));
    ndcg=@(r,k)dcg(r,k)./idcg(r,k);
    
    R=@(g,gmax)(2.^g-1)./(2.^gmax);
    err=@(r,k,gmax)sum((1./(1:k)).*R(r(1:k),gmax).*prod(tril(repmat(1-R(r(1:k),gmax),k,1),-1)+triu(ones(k)),2)');
    
    y = zeros(1,5);
    y(1) = p(r,k,gmax);
    y(2) = cg(r,k);
    y(3) = dcg(r,k);
    y(4) = ndcg(r,k);
    y(5) = err(r,k,gmax);

end
