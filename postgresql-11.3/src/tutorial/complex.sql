---------------------------------------------------------------------------
--
-- complex.sql-
--    This file shows how to create a new user-defined type and how to
--    use this new type.
--
--
-- Portions Copyright (c) 1996-2018, PostgreSQL Global Development Group
-- Portions Copyright (c) 1994, Regents of the University of California
--
-- src/tutorial/complex.source
--
---------------------------------------------------------------------------

-----------------------------
-- Creating a new type:
-- We are going to create a new type called 'complex' which represents
-- complex numbers.
-- A user-defined type must have an input and an output function, and
-- optionally can have binary input and output functions.  All of these
-- are usually user-defined C functions.
-----------------------------

-- Assume the user defined functions are in /Users/Taru/Desktop/9315/postgresql-11.3/src/tutorial/complex$DLSUFFIX
-- (we do not want to assume this is in the dynamic loader search path).
-- Look at $PWD/complex.c for the source.  Note that we declare all of
-- them as STRICT, so we do not need to cope with NULL inputs in the
-- C code.  We also mark them IMMUTABLE, since they always return the
-- same outputs given the same inputs.

-- the input function 'complex_in' takes a null-terminated string (the
-- textual representation of the type) and turns it into the internal
-- (in memory) representation. You will get a message telling you 'complex'
-- does not exist yet but that's okay.
drop FUNCTION email_in;
drop FUNCTION email_out;
CREATE FUNCTION email_in(cstring)
   RETURNS EmailAddr
   AS '_OBJWD_/email'
   LANGUAGE C IMMUTABLE STRICT;

CREATE or replace FUNCTION email_out(EmailAddr)
   RETURNS cstring
   AS '_OBJWD_/email'
   LANGUAGE C IMMUTABLE STRICT;
CREATE TYPE EmailAddr (
   input = email_in,
   output = email_out,
   INTERNALLENGTH = VARIABLE
  
);
CREATE FUNCTION email_lt(EmailAddr, EmailAddr) RETURNS bool
   AS '_OBJWD_/email' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION email_le(EmailAddr, EmailAddr) RETURNS bool
   AS '_OBJWD_/email' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION email_eq(EmailAddr, EmailAddr) RETURNS bool
   AS '_OBJWD_/email' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION email_ne(EmailAddr, EmailAddr) RETURNS bool
   AS '_OBJWD_/email' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION email_ge(EmailAddr, EmailAddr) RETURNS bool
   AS '_OBJWD_/email' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION email_gt(EmailAddr, EmailAddr) RETURNS bool
   AS '_OBJWD_/email' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION email_de(EmailAddr, EmailAddr) RETURNS bool
   AS '_OBJWD_/email' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION email_dne(EmailAddr, EmailAddr) RETURNS bool
   AS '_OBJWD_/email' LANGUAGE C IMMUTABLE STRICT;
create FUNCTION email_hash(EmailAddr) RETURNS integer
   AS '_OBJWD_/email' LANGUAGE C IMMUTABLE STRICT;




CREATE OPERATOR < (
   leftarg = EmailAddr, rightarg = EmailAddr, procedure = email_lt,
   commutator = > , negator = >= ,
   restrict = scalarltsel, join = scalarltjoinsel
);

CREATE OPERATOR <= (
   leftarg = EmailAddr, rightarg = EmailAddr, procedure = email_le,
   commutator = >= , negator = > ,
   restrict = scalarltsel, join = scalarltjoinsel
);
CREATE OPERATOR = (
   leftarg = EmailAddr, rightarg = EmailAddr, procedure = email_eq,
   commutator = = , negator  = <>,
   restrict = eqsel, join = eqjoinsel,MERGES,HASHES
);
CREATE OPERATOR <> (
   leftarg = EmailAddr, rightarg = EmailAddr, procedure = email_ne,
   commutator = <> , negator = = ,
   restrict = neqsel, join = neqjoinsel
);
CREATE OPERATOR >= (
   leftarg = EmailAddr, rightarg = EmailAddr, procedure = email_ge,
   commutator = <= , negator = < ,
   restrict = scalarltsel, join = scalarltjoinsel
);
CREATE OPERATOR > (
   leftarg = EmailAddr, rightarg = EmailAddr, procedure = email_gt,
   commutator = < , negator = <= ,
   restrict = scalarltsel, join = scalarltjoinsel
);
CREATE OPERATOR ~ (
   leftarg = EmailAddr, rightarg = EmailAddr, procedure = email_de,
   commutator = ~ , negator = !~,
   restrict = eqsel, join = eqjoinsel
);
CREATE OPERATOR !~ (
   leftarg = EmailAddr, rightarg = EmailAddr, procedure = email_dne,
   commutator = !~ , negator = ~,
   restrict = neqsel, join = neqjoinsel
);
CREATE OPERATOR CLASS email_ops DEFAULT FOR TYPE EmailAddr USING hash AS 
      OPERATOR 1  = (EmailAddr, EmailAddr),
      FUNCTION 1  email_hash(EmailAddr);



create table UserSessions (
   username  EmailAddr,
   loggedIn  timestamp,
   loggedOut timestamp
   -- etc. etc.
);
insert into UserSessions(username,loggedIn,loggedOut) values
('jas@cse.unsw.edu.au','2012-07-01 15:45:55','2012-07-01 15:51:20'),
('jas@cse.unsw.EDU.AU','2012-07-01 15:50:30','2012-07-01 15:53:15'),
('z9987654@unsw.edu.au','2012-07-01 15:51:10','2012-07-01 16:01:05'),
('m.mouse@disney.com','2012-07-01 15:51:11','2012-07-01 16:01:06'),
('a-user@fast-money.com','2012-07-01 15:52:25','2012-07-01 16:10:15');

select * from UserSessions;
