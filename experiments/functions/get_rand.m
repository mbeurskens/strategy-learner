function [ x ] = get_rand( xmin, xmax )
%GET_RAND Get random number using uniform distribution in range xmin:xmax
x=xmin+rand(1)*(xmax-xmin);
end

