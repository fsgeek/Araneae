# Hobo

The Hobo Spider is now found in the Pacific Northwest, but was introduced to the area (hence it is an invasive species) in
the 1980s.  It is a form of funnel web spider.

This _project_ is really about exploring possible ways of presenting information through the existing interface.  It's likely
to disappear and move on at some point.

# Documentation


# Design Notes

##  Database

The underlying logical storage is based upon an in-memory database called WhiteDB.  The choice is somewhat arbitrary and I suspect in the end I'll want to build a custom database for this project,
largely due to performance considerations, but also because that's what a file system _is_ at its heart: a highly customized database.

The point here is to focus on _relationships_.  I don't care about data storage, I'm going to leave that to other file systems.  What I _do_ care about are relationships.  I don't care what the relationship
is per se, but I do care about finding and showing those relationships.  So the primary record class is the _relationship_.  

A relationship is a tuple: (object, object, labels, properties, attributes).  At this point I'm not defining what these _are_, since I think that will evolve as my understanding improves.  But initially,
my goal is to provide some basic form to capture the initial relationships that we've been exploring:

* contains (or contained by)
* points to (link) - this is what we point to and consists of a _Uniform Resource Identifier_ (URI).
* similar to - so this probably needs to identify the _type_ of similarity as well as some metric or value that captures the similarity.  This could capture temporal similarity, as well as content similarity.


Properties would include:

* hash - this is a hash value of the content(s).  This changes as the underlying object changes, so we may need to invalidate this when the content changes
* stat - this is the stat information for the object: size and timestamps.
* name - this would be a label applied to the given relationship and is optional (question: how do we capture _name_ similarity in this way?)


WhiteDB required that I define encodings for these values.

One of my objectives here is to eliminate directories (if at all possible).  Since I'm mapping this to a hierarchical interface, what that generally means is that I'm focusing on the _relationships_ to capture
that state. 

So an object consists of at least:

* Type - this is the type of the record.  I'll have to define these (of course).  This is a non-unique field.
* UUID - this is the UUID of the record.  These are unique and should be indexed.
* Change Time - this is a timestamp recording the last time this record was changed (this is the same as the Modify Time from a file systems perspective)
* Access Time - this is a timestamp recording the last time this record was accessed (TBD: is this necessary/useful?  We know this creates a lot of extra traffic - may want to limit the granularity of this)
* URI - where this object is located.  This would include a URL (for a web resource) and a path name (for a local file system resource).  I'm sure there will be others as well (e.g., a database resource!)

Different types of objects are likely to have additional fields. Note that a limitation of WhiteDB is that once you define the size of a record (the number of fields it contains) you cannot change it.  So 
this likely becomes a function of the _type_ of the record.

WhiteDB requires functions to "encode" and "decode" these structures.  Thus, I'm thinking the logical way to lay this out is to have a common header (for all database objects) and then a specialized portion
(for the specific type of object).  Again, I'm going to have to play with this to see what "makes sense".

Note: I have confirmed that I cannot search on a binary UUID; either the underlying database needs to be modified to support them or I need to convert them to strings.  Since I doubt I'll be using this
database package in the long run, I'm going to just encode them as string encodings for the time being.

Last Update: August 28, 2019.
