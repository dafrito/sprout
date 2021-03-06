-- Methods that provide currying, a common functional programming idiom that allows one
-- to partially apply a method.
--
-- Most functions in this addon use Curry implicitly, so you don't need to call it yourself.
--
-- TODO Write more about currying, since we use it everywhere.
--
-- Using setfenv with curried functions must be done carefully. For one, builtins do not
-- support setfenv (they will throw errors when setfenv is used). As a result, currying
-- cannot reasonably forward a curried function that has been setfenv'd to its underlying
-- counterpart.
--
-- To get the behavior you want, you must use setfenv on the underlying function, NOT the
-- returned curried function; using setfenv on a curried function will have no effect.

if nil ~= require then
	require "fritomod/basic";
end;

-- Curries the specified method or function using any specified arguments. This will make
-- a best-effort approach to determine what method to invoke.
--
-- objOrFunc must not be a callable table. Such things are too ambiguous to be reliably
-- coerced into a method or function, so we just disallow them outright. Use CurryMethod or
-- CurryFunction to get the behavior you need.
--
-- objOrFunc:function, table, string
--	 Either:
--	 a. A reference to a function. CurryFunction will be used.
--	 b. A non-callable table. CurryMethod will be used.
--	 c. A string that is the name of a method. CurryHeadlessMethod will be used.
-- funcOrName:*
--	 If objOrFunc is a table, then this is a string referring to a method, or a method
--	 reference.
--	 If objOrfunc is a function, then this is simply the first argument.
-- ...
--	 optional. Any number of non-nil arguments that are curried to the specified method
--	 or function.
-- returns
--	 a partially applied method that, when invoked, will call the specified function or method,
--	 passing along partially-applied arguments and subsequent arguments, in that order. It will
--	 return the value returned by the specified function or method
-- throws
--	 if objOrFunc is a callable table
--	 if objOrFunc is not a string, table, or function
--	 if any arguments, either curried or passed, are nil
function Curry(...)
	if select("#", ...) == 1 then
		local potentialCallable = select(1, ...);
		if IsCallable(potentialCallable) then
			return potentialCallable;
		end;
		if type(potentialCallable) == "string" then
			return CurryHeadlessMethod(potentialCallable);
		end;
	end;
	local objOrFunc, funcOrName = ...;
	if not objOrFunc then
		error("objOrFunc is falsy");
	end;
	if type(objOrFunc) == "function" then
		return CurryFunction(...);
	end;
	if type(objOrFunc) == "table" or type(objOrFunc) == "userdata" then
		local metatable = getmetatable(objOrFunc);
		if type(metatable) == "table" and IsCallable(metatable.__call) then
			-- Callable tables are too ambiguous to be implicitly curried. If you need to curry
			-- a callable table, use the CurryMethod or CurryFunction methods explicitly.
			error("objOrFunc is a callable table and therefore ambiguous.");
		end;
		return CurryMethod(...);
	end;
	if type(objOrFunc) == "string" then
		return CurryHeadlessMethod(...);
	end;
	error("objOrFunc is not a string, table, or function. Received type: " .. type(objOrFunc));
end;

-- Partially applies the specified function using the specified arguments, returning a function that
-- will apply the specified function using any provided arguments.
--
-- For example, all examples are equivalent:
-- a.) Naive call
-- foo(a, b, c, d)
--
-- b.) Curried call
-- curried = CurryFunction(foo, a, b)
-- curried(c, d) -- invokes foo(a, b, c, d)
--
-- func:function
--	 called in the returned function
-- ...
--	 optional. Any non-nil arguments that should be partially applied for the specified function
-- returns:function
--	 a function that will invoke the partially applied function with any passed arguments. It will
--	 return the value returned by the paritally applied function.
-- throws
--	 if func is not callable
--	 if any curried arguments are nil
--	 if passed arguments are nil while also having curried arguments
function CurryFunction(func, ...)
	if not func then
		error("func is falsy");
	end;
	if not IsCallable(func) then
		error("func is not a function or string. Received type: %s", func);
	end;
	local numArgs = select("#", ...);
	-- These elseif statements are autogenerated from the bin/write-curryables.lua
	-- script. Please edit that script and c/p them here, rather than editing these
	-- entries inline.
	if numArgs == 0 then
		return func;
	elseif numArgs == 1 then
		local a1=...;
		return function(...)
			return func(a1, ...);
		end;
	elseif numArgs == 2 then
		local a1, a2=...;
		return function(...)
			return func(a1, a2, ...);
		end;
	elseif numArgs == 3 then
		local a1, a2, a3=...;
		return function(...)
			return func(a1, a2, a3, ...);
		end;
	elseif numArgs == 4 then
		local a1, a2, a3, a4=...;
		return function(...)
			return func(a1, a2, a3, a4, ...);
		end;
	elseif numArgs == 5 then
		local a1, a2, a3, a4, a5=...;
		return function(...)
			return func(a1, a2, a3, a4, a5, ...);
		end;
	elseif numArgs == 6 then
		local a1, a2, a3, a4, a5, a6=...;
		return function(...)
			return func(a1, a2, a3, a4, a5, a6, ...);
		end;
	elseif numArgs == 7 then
		local a1, a2, a3, a4, a5, a6, a7=...;
		return function(...)
			return func(a1, a2, a3, a4, a5, a6, a7, ...);
		end;
	elseif numArgs == 8 then
		local a1, a2, a3, a4, a5, a6, a7, a8=...;
		return function(...)
			return func(a1, a2, a3, a4, a5, a6, a7, a8, ...);
		end;
	elseif numArgs == 9 then
		local a1, a2, a3, a4, a5, a6, a7, a8, a9=...;
		return function(...)
			return func(a1, a2, a3, a4, a5, a6, a7, a8, a9, ...);
		end;
	elseif numArgs == 10 then
		local a1, a2, a3, a4, a5, a6, a7, a8, a9, a10=...;
		return function(...)
			return func(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, ...);
		end;
	elseif numArgs == 11 then
		local a1, a2, a3, a4, a5, a6, a7, a8, a9,
			a10, a11=...;
		return function(...)
			return func(a1, a2, a3, a4, a5, a6, a7, a8, a9,
				a10, a11, ...);
		end;
	elseif numArgs == 12 then
		local a1, a2, a3, a4, a5, a6, a7, a8, a9,
			a10, a11, a12=...;
		return function(...)
			return func(a1, a2, a3, a4, a5, a6, a7, a8, a9,
				a10, a11, a12, ...);
		end;
	elseif numArgs == 13 then
		local a1, a2, a3, a4, a5, a6, a7, a8, a9,
			a10, a11, a12, a13=...;
		return function(...)
			return func(a1, a2, a3, a4, a5, a6, a7, a8, a9,
				a10, a11, a12, a13, ...);
		end;
	elseif numArgs == 14 then
		local a1, a2, a3, a4, a5, a6, a7, a8, a9,
			a10, a11, a12, a13, a14=...;
		return function(...)
			return func(a1, a2, a3, a4, a5, a6, a7, a8, a9,
				a10, a11, a12, a13, a14, ...);
		end;
	elseif numArgs == 15 then
		local a1, a2, a3, a4, a5, a6, a7, a8, a9,
			a10, a11, a12, a13, a14, a15=...;
		return function(...)
			return func(a1, a2, a3, a4, a5, a6, a7, a8, a9,
				a10, a11, a12, a13, a14, a15, ...);
		end;
	elseif numArgs == 16 then
		local a1, a2, a3, a4, a5, a6, a7, a8, a9,
			a10, a11, a12, a13, a14, a15, a16=...;
		return function(...)
			return func(a1, a2, a3, a4, a5, a6, a7, a8, a9,
				a10, a11, a12, a13, a14, a15, a16, ...);
		end;
	elseif numArgs == 17 then
		local a1, a2, a3, a4, a5, a6, a7, a8, a9,
			a10, a11, a12, a13, a14, a15, a16, a17=...;
		return function(...)
			return func(a1, a2, a3, a4, a5, a6, a7, a8, a9,
				a10, a11, a12, a13, a14, a15, a16, a17, ...);
		end;
	elseif numArgs == 18 then
		local a1, a2, a3, a4, a5, a6, a7, a8, a9,
			a10, a11, a12, a13, a14, a15, a16, a17, a18=...;
		return function(...)
			return func(a1, a2, a3, a4, a5, a6, a7, a8, a9,
				a10, a11, a12, a13, a14, a15, a16, a17, a18, ...);
		end;
	elseif numArgs == 19 then
		local a1, a2, a3, a4, a5, a6, a7, a8, a9,
			a10, a11, a12, a13, a14, a15, a16, a17, a18, a19=...;
		return function(...)
			return func(a1, a2, a3, a4, a5, a6, a7, a8, a9,
				a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, ...);
		end;
	elseif numArgs == 20 then
		local a1, a2, a3, a4, a5, a6, a7, a8, a9,
			a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20=...;
		return function(...)
			return func(a1, a2, a3, a4, a5, a6, a7, a8, a9,
				a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, ...);
		end;
	elseif numArgs == 21 then
		local a1, a2, a3, a4, a5, a6, a7, a8, a9,
			a10, a11, a12, a13, a14, a15, a16, a17, a18, a19,
			a20, a21=...;
		return function(...)
			return func(a1, a2, a3, a4, a5, a6, a7, a8, a9,
				a10, a11, a12, a13, a14, a15, a16, a17, a18, a19,
				a20, a21, ...);
		end;
	elseif numArgs == 22 then
		local a1, a2, a3, a4, a5, a6, a7, a8, a9,
			a10, a11, a12, a13, a14, a15, a16, a17, a18, a19,
			a20, a21, a22=...;
		return function(...)
			return func(a1, a2, a3, a4, a5, a6, a7, a8, a9,
				a10, a11, a12, a13, a14, a15, a16, a17, a18, a19,
				a20, a21, a22, ...);
		end;
	elseif numArgs == 23 then
		local a1, a2, a3, a4, a5, a6, a7, a8, a9,
			a10, a11, a12, a13, a14, a15, a16, a17, a18, a19,
			a20, a21, a22, a23=...;
		return function(...)
			return func(a1, a2, a3, a4, a5, a6, a7, a8, a9,
				a10, a11, a12, a13, a14, a15, a16, a17, a18, a19,
				a20, a21, a22, a23, ...);
		end;
	elseif numArgs == 24 then
		local a1, a2, a3, a4, a5, a6, a7, a8, a9,
			a10, a11, a12, a13, a14, a15, a16, a17, a18, a19,
			a20, a21, a22, a23, a24=...;
		return function(...)
			return func(a1, a2, a3, a4, a5, a6, a7, a8, a9,
				a10, a11, a12, a13, a14, a15, a16, a17, a18, a19,
				a20, a21, a22, a23, a24, ...);
		end;
	elseif numArgs == 25 then
		local a1, a2, a3, a4, a5, a6, a7, a8, a9,
			a10, a11, a12, a13, a14, a15, a16, a17, a18, a19,
			a20, a21, a22, a23, a24, a25=...;
		return function(...)
			return func(a1, a2, a3, a4, a5, a6, a7, a8, a9,
				a10, a11, a12, a13, a14, a15, a16, a17, a18, a19,
				a20, a21, a22, a23, a24, a25, ...);
		end;
	elseif numArgs == 26 then
		local a1, a2, a3, a4, a5, a6, a7, a8, a9,
			a10, a11, a12, a13, a14, a15, a16, a17, a18, a19,
			a20, a21, a22, a23, a24, a25, a26=...;
		return function(...)
			return func(a1, a2, a3, a4, a5, a6, a7, a8, a9,
				a10, a11, a12, a13, a14, a15, a16, a17, a18, a19,
				a20, a21, a22, a23, a24, a25, a26, ...);
		end;
	elseif numArgs == 27 then
		local a1, a2, a3, a4, a5, a6, a7, a8, a9,
			a10, a11, a12, a13, a14, a15, a16, a17, a18, a19,
			a20, a21, a22, a23, a24, a25, a26, a27=...;
		return function(...)
			return func(a1, a2, a3, a4, a5, a6, a7, a8, a9,
				a10, a11, a12, a13, a14, a15, a16, a17, a18, a19,
				a20, a21, a22, a23, a24, a25, a26, a27, ...);
		end;
	elseif numArgs == 28 then
		local a1, a2, a3, a4, a5, a6, a7, a8, a9,
			a10, a11, a12, a13, a14, a15, a16, a17, a18, a19,
			a20, a21, a22, a23, a24, a25, a26, a27, a28=...;
		return function(...)
			return func(a1, a2, a3, a4, a5, a6, a7, a8, a9,
				a10, a11, a12, a13, a14, a15, a16, a17, a18, a19,
				a20, a21, a22, a23, a24, a25, a26, a27, a28, ...);
		end;
	elseif numArgs == 29 then
		local a1, a2, a3, a4, a5, a6, a7, a8, a9,
			a10, a11, a12, a13, a14, a15, a16, a17, a18, a19,
			a20, a21, a22, a23, a24, a25, a26, a27, a28, a29=...;
		return function(...)
			return func(a1, a2, a3, a4, a5, a6, a7, a8, a9,
				a10, a11, a12, a13, a14, a15, a16, a17, a18, a19,
				a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, ...);
		end;
	elseif numArgs == 30 then
		local a1, a2, a3, a4, a5, a6, a7, a8, a9,
			a10, a11, a12, a13, a14, a15, a16, a17, a18, a19,
			a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30=...;
		return function(...)
			return func(a1, a2, a3, a4, a5, a6, a7, a8, a9,
				a10, a11, a12, a13, a14, a15, a16, a17, a18, a19,
				a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, ...);
		end;
	elseif numArgs == 31 then
		local a1, a2, a3, a4, a5, a6, a7, a8, a9,
			a10, a11, a12, a13, a14, a15, a16, a17, a18, a19,
			a20, a21, a22, a23, a24, a25, a26, a27, a28, a29,
			a30, a31=...;
		return function(...)
			return func(a1, a2, a3, a4, a5, a6, a7, a8, a9,
				a10, a11, a12, a13, a14, a15, a16, a17, a18, a19,
				a20, a21, a22, a23, a24, a25, a26, a27, a28, a29,
				a30, a31, ...);
		end;
	elseif numArgs == 32 then
		local a1, a2, a3, a4, a5, a6, a7, a8, a9,
			a10, a11, a12, a13, a14, a15, a16, a17, a18, a19,
			a20, a21, a22, a23, a24, a25, a26, a27, a28, a29,
			a30, a31, a32=...;
		return function(...)
			return func(a1, a2, a3, a4, a5, a6, a7, a8, a9,
				a10, a11, a12, a13, a14, a15, a16, a17, a18, a19,
				a20, a21, a22, a23, a24, a25, a26, a27, a28, a29,
				a30, a31, a32, ...);
		end;
	elseif numArgs == 33 then
		local a1, a2, a3, a4, a5, a6, a7, a8, a9,
			a10, a11, a12, a13, a14, a15, a16, a17, a18, a19,
			a20, a21, a22, a23, a24, a25, a26, a27, a28, a29,
			a30, a31, a32, a33=...;
		return function(...)
			return func(a1, a2, a3, a4, a5, a6, a7, a8, a9,
				a10, a11, a12, a13, a14, a15, a16, a17, a18, a19,
				a20, a21, a22, a23, a24, a25, a26, a27, a28, a29,
				a30, a31, a32, a33, ...);
		end;
	elseif numArgs == 34 then
		local a1, a2, a3, a4, a5, a6, a7, a8, a9,
			a10, a11, a12, a13, a14, a15, a16, a17, a18, a19,
			a20, a21, a22, a23, a24, a25, a26, a27, a28, a29,
			a30, a31, a32, a33, a34=...;
		return function(...)
			return func(a1, a2, a3, a4, a5, a6, a7, a8, a9,
				a10, a11, a12, a13, a14, a15, a16, a17, a18, a19,
				a20, a21, a22, a23, a24, a25, a26, a27, a28, a29,
				a30, a31, a32, a33, a34, ...);
		end;
	elseif numArgs == 35 then
		local a1, a2, a3, a4, a5, a6, a7, a8, a9,
			a10, a11, a12, a13, a14, a15, a16, a17, a18, a19,
			a20, a21, a22, a23, a24, a25, a26, a27, a28, a29,
			a30, a31, a32, a33, a34, a35=...;
		return function(...)
			return func(a1, a2, a3, a4, a5, a6, a7, a8, a9,
				a10, a11, a12, a13, a14, a15, a16, a17, a18, a19,
				a20, a21, a22, a23, a24, a25, a26, a27, a28, a29,
				a30, a31, a32, a33, a34, a35, ...);
		end;
	elseif numArgs == 36 then
		local a1, a2, a3, a4, a5, a6, a7, a8, a9,
			a10, a11, a12, a13, a14, a15, a16, a17, a18, a19,
			a20, a21, a22, a23, a24, a25, a26, a27, a28, a29,
			a30, a31, a32, a33, a34, a35, a36=...;
		return function(...)
			return func(a1, a2, a3, a4, a5, a6, a7, a8, a9,
				a10, a11, a12, a13, a14, a15, a16, a17, a18, a19,
				a20, a21, a22, a23, a24, a25, a26, a27, a28, a29,
				a30, a31, a32, a33, a34, a35, a36, ...);
		end;
	elseif numArgs == 37 then
		local a1, a2, a3, a4, a5, a6, a7, a8, a9,
			a10, a11, a12, a13, a14, a15, a16, a17, a18, a19,
			a20, a21, a22, a23, a24, a25, a26, a27, a28, a29,
			a30, a31, a32, a33, a34, a35, a36, a37=...;
		return function(...)
			return func(a1, a2, a3, a4, a5, a6, a7, a8, a9,
				a10, a11, a12, a13, a14, a15, a16, a17, a18, a19,
				a20, a21, a22, a23, a24, a25, a26, a27, a28, a29,
				a30, a31, a32, a33, a34, a35, a36, a37, ...);
		end;
	elseif numArgs == 38 then
		local a1, a2, a3, a4, a5, a6, a7, a8, a9,
			a10, a11, a12, a13, a14, a15, a16, a17, a18, a19,
			a20, a21, a22, a23, a24, a25, a26, a27, a28, a29,
			a30, a31, a32, a33, a34, a35, a36, a37, a38=...;
		return function(...)
			return func(a1, a2, a3, a4, a5, a6, a7, a8, a9,
				a10, a11, a12, a13, a14, a15, a16, a17, a18, a19,
				a20, a21, a22, a23, a24, a25, a26, a27, a28, a29,
				a30, a31, a32, a33, a34, a35, a36, a37, a38, ...);
		end;
	elseif numArgs == 39 then
		local a1, a2, a3, a4, a5, a6, a7, a8, a9,
			a10, a11, a12, a13, a14, a15, a16, a17, a18, a19,
			a20, a21, a22, a23, a24, a25, a26, a27, a28, a29,
			a30, a31, a32, a33, a34, a35, a36, a37, a38, a39=...;
		return function(...)
			return func(a1, a2, a3, a4, a5, a6, a7, a8, a9,
				a10, a11, a12, a13, a14, a15, a16, a17, a18, a19,
				a20, a21, a22, a23, a24, a25, a26, a27, a28, a29,
				a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, ...);
		end;
	elseif numArgs == 40 then
		local a1, a2, a3, a4, a5, a6, a7, a8, a9,
			a10, a11, a12, a13, a14, a15, a16, a17, a18, a19,
			a20, a21, a22, a23, a24, a25, a26, a27, a28, a29,
			a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40=...;
		return function(...)
			return func(a1, a2, a3, a4, a5, a6, a7, a8, a9,
				a10, a11, a12, a13, a14, a15, a16, a17, a18, a19,
				a20, a21, a22, a23, a24, a25, a26, a27, a28, a29,
				a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, ...);
		end;
	else
		-- Since we've gone beyond the inlined cases, we're forced to use tables to
		-- contain these arguments. Tables do not handle nil values in an array very well,
		-- so we disallow them here. This shouldn't occur often since we have so many inlined
		-- cases, but I think it's important to fail somewhat predictably here.
		for i=1, numArgs do
			if select(i, ...)==nil then
				error(("Argument #%d must not be nil"):format(i));
			end;
		end;
		local args = {...};
		return function(...)
			local n=select("#", ...);
			local foundNil=false;
			for i=1,n do
				if select(i, ...)==nil then
					foundNil=true;
				elseif foundNil then
					error(("Argument #%d was nil, but the following argument was not."):format(i-1));
				end;
			end;
			return func(UnpackAll(args, {...}));
		end;
	end;
end

-- Returns a partially applied function that, when invoked, calls the function on the specified
-- object with the specified name. This is different from CurryMethod in that this function does
-- not automatically include the object as the first argument.
--
-- Since this never caches the actual function, the returned function stays updated with the
-- specified object. It is therefore useful if you wish to succinctly refer to a changing method.
--
-- object:table
--	 the object that will contain the specified named function
-- name:*
--	 the name of the function
-- ...
--	 optional. Any number of non-nil arguments that are partially applied to the specified function
-- returns:function
--	 a function that, when invoked, invokes the specified named function on the specified
--	 object
-- throws
--	 if obj or name is falsy
--	 if, when invoked, there is no value for the specified name on the specified object
--	 if, when invoked, the value at the specified name is not callable
--	 if any curried or passed arguments are nil
function CurryNamedFunction(obj, name, ...)
	assert(obj, "obj is falsy");
	assert(name, "name is falsy");
	return CurryFunction(function(...)
		local func = obj[name];
		if func==nil then
			error("Named function was not found. Name: " .. name);
		end;
		assert(IsCallable(func), "Named function is not callable.");
		return func(...);
	end, ...);
end;

-- Returns a partially applied method that, when invoked, calls a method on the specified
-- object.
--
-- For example, all examples are equivalent:
-- a.) Naive call
-- obj:foo(a, b, c, d);
--
-- b.) Reference-based method currying
-- curried = CurryMethod(obj, foo, a, b);
-- curried(c, d);
--
-- c.) String-based method currying
-- curried = CurryMethod(obj, "foo", a, b);
-- curried(c, d);
--
-- object:table
--	 the object that holds the curried method
-- func:*
--	 a reference to a method, or the name of the method that will be invoked
-- ...
--	 optional. Any arguments that are partially applied to the specified method
-- returns:function
--	 a partially applied method that, when invoked, will call the specified method, passing
--	 along partially-applied arguments and subsequent arguments, in that order. It will return
--	 the value returned by the specified method
-- throws
--	 if any curried or passed arguments are nil
function CurryMethod(object, func, ...)
	assert(object, "object must not be falsy during currying");
	assert(func, "func must not be falsy during currying");
	if type(object) ~= "table" and type(object) ~= "userdata" then
		error(("object is not a table. Received type: %s"):format(type(object)));
	end;
	if IsCallable(func) then
		return CurryFunction(func, object, ...);
	elseif type(func) == "string" then
		local name=func;
		return CurryFunction(function(...)
			local func=object[name];
			if func==nil then
				error("Named function was not found. Name: " .. name);
			end;
			assert(IsCallable(func), "Named function is not callable.");
			return func(object, ...);
		end, ...);
	end;
	error("func is not callable and is not a string. Received type: " .. type(func));
end

-- Curries the specified headless method using the specified arguments, returning a callable
-- that represents the curried headless method. On invocation, the returned callable will use
-- the first argument as the "self" for the headless method.
--
-- This is useful for times where you want a method to be called on a group of objects. Many methods
-- in utility methods use this method for that purpose.
--
-- a.) Reference-based method currying
-- curried = CurryHeadlessMethod(foo, a, b);
-- curried(obj, c, d); -- invokes foo(obj, a, b, c, d);
--
-- b.) String-based method currying
-- curried = CurryHeadlessMethod("foo", a, b);
-- curried(obj, c, d); -- invokes foo(obj, a, b, c, d);
--
-- func:*
--	 A reference to a method, or a string representing the name of the method, that is curried.
-- ...
--	 Any arguments that should be passed, in order, before subsequent arguments, to
--	 func. These are optional.
-- returns:function
--	 a partially applied method that, when invoked, will call the specified method. The first argument
--	 provided should be the self reference. This reference will be passed first to the specified method,
--	 followed by partially-applied arguments, and finally arguments passed to the returned function. It
--	 will return the value returned by the specified method.
-- throws
--	 if any curried or passed arguments are nil
function CurryHeadlessMethod(func, ...)
	assert(func, "func must not be falsy");
	local invokedSelf;
	local invokeHeadless = CurryFunction(function(...)
		if IsCallable(func) then
			return func(invokedSelf, ...);
		else
			return invokedSelf[func](invokedSelf, ...);
		end;
	end, ...);
	return function(self, ...)
		invokedSelf=self;
		return invokeHeadless(...);
	end;
end;
Headless=CurryHeadlessMethod;

-- Returns a method that ignores any arguments passed to it, only invoking the specified
-- function with its curried arguments. In effect, this creates a sealed function that is
-- fully applied. Sealed functions are useful when you wish to use a fully applied function
-- in a situation where arguments may be unnecessarily passed to it.
--
-- func(a, b); -- Calls func(a, b)
-- curried = Seal(func, a, b);
-- curried(); -- Calls func(a, b)
-- curried(c, d); -- Still calls func(a, b)
--
-- func
--	 the function that will be sealed by this method
-- ...
--	  any arguments that will be passed to func when the returned, sealed function is invoked
-- returns
--	  a function that will invoke func(...), ignoring any immediate arguments passed to it.
-- throws
--	  if func is not callable
function Seal(func, ...)
	func = Curry(func, ...);
	return function()
		return func();
	end;
end;

-- Curries the given function, returning a function that calls that function without passing
-- additional arguments. If additional arguments are given to the returned function, an assertion
-- is raised.
--
-- func, ...
--	 the curried function that is sealed by this operation
-- returns:function
--	 a function that, when called, invokes the curried function. If the returned function is
--	 passed any arguments, an assertion is raised
-- throws
--	 if arguments are passed to the returned function
function ForcedSeal(func, ...)
	func = Curry(func, ...);
	return function(...)
		assert(select("#", ...) == 0, "Force-sealed function must not be passed arguments");
		return func();
	end;
end;

-- Returns a function that calls the specified function. The returned function guarantees that
-- the specified self argument is always used as the self argument for the function. If one is
-- provided, it is ignored.
--
-- self
--	 the self argument that is guaranteed to be the self argument for the specified function
-- func, ...
--	 the function that is invoked by the returned function
-- returns
--	 a function that behaves as described above
-- throws
--	 if self is falsy or not a table
-- see
--	 ForcedFunction
function ForcedMethod(self, func, ...)
	assert(self, "self is falsy");
	assert(type(self) == "table", "self is not a table. Type: " .. type(self));
	func = CurryMethod(self, func, ...);
	return function(maybeSelf, ...)
		if maybeSelf == self then
			return func(...);
		end;
		-- It's just another argument, so include it.
		return func(maybeSelf, ...);
	end;
end;

-- Returns a function that calls the specified function. The returned function guarantees that
-- the specified self argument is never used as the self argument for the function. If one is
-- provided, it is omitted.
--
-- self
--	 the self argument that is ignored
-- func, ...
--	 the function that is invoked by the returned function
-- returns
--	 a function that behaves as described above
-- throws
--	 if self is falsy or not a table
-- see
--	 ForcedMethod
function ForcedFunction(self, func, ...)
	assert(self, "self is falsy");
	assert(type(self) == "table", "self is not a table. Type: " .. type(self));
	func = Curry(func, ...);
	return function(maybeSelf, ...)
		if maybeSelf == self then
			return func(...);
		end;
		-- It's just another argument, so include it.
		return func(maybeSelf, ...);
	end;
end;

-- Strip the first argument before invoking the specified function. This is useful for overriding
-- methods via forwarding to another object.
function Override(func, ...)
	local curried = Curry(func, ...);
	return function(_, ...)
		return curried(...);
	end;
end;

-- vim: set noet :
