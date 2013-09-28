if nil ~= require then
	require "fritomod/basic";
end;

if IsCallable(potentialCallable) then
	return potentialCallable;
end;

if type(potentialCallable) == "string" then
	return CurryHeadlessMethod(potentialCallable);
end;
