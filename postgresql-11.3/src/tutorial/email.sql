CREATE FUNCTION email_in(cstring)
   RETURNS EmailAddr
   AS '/Users/Taru/Desktop/9315/postgresql-11.3/src/tutorial/email'
   LANGUAGE C IMMUTABLE STRICT;

CREATE or replace FUNCTION email_out(EmailAddr)
   RETURNS cstring
   AS '/Users/Taru/Desktop/9315/postgresql-11.3/src/tutorial/email'
   LANGUAGE C IMMUTABLE STRICT;
CREATE TYPE EmailAddr (
   input = email_in,
   output = email_out,
   INTERNALLENGTH = VARIABLE
  
);
CREATE FUNCTION email_lt(EmailAddr, EmailAddr) RETURNS bool
   AS '/Users/Taru/Desktop/9315/postgresql-11.3/src/tutorial/email' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION email_le(EmailAddr, EmailAddr) RETURNS bool
   AS '/Users/Taru/Desktop/9315/postgresql-11.3/src/tutorial/email' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION email_eq(EmailAddr, EmailAddr) RETURNS bool
   AS '/Users/Taru/Desktop/9315/postgresql-11.3/src/tutorial/email' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION email_ne(EmailAddr, EmailAddr) RETURNS bool
   AS '/Users/Taru/Desktop/9315/postgresql-11.3/src/tutorial/email' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION email_ge(EmailAddr, EmailAddr) RETURNS bool
   AS '/Users/Taru/Desktop/9315/postgresql-11.3/src/tutorial/email' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION email_gt(EmailAddr, EmailAddr) RETURNS bool
   AS '/Users/Taru/Desktop/9315/postgresql-11.3/src/tutorial/email' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION email_de(EmailAddr, EmailAddr) RETURNS bool
   AS '/Users/Taru/Desktop/9315/postgresql-11.3/src/tutorial/email' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION email_dne(EmailAddr, EmailAddr) RETURNS bool
   AS '/Users/Taru/Desktop/9315/postgresql-11.3/src/tutorial/email' LANGUAGE C IMMUTABLE STRICT;
create FUNCTION email_hash(EmailAddr) RETURNS integer
   AS '/Users/Taru/Desktop/9315/postgresql-11.3/src/tutorial/email' LANGUAGE C IMMUTABLE STRICT;
create FUNCTION email_cmp(EmailAddr,EmailAddr) RETURNS integer
   AS '/Users/Taru/Desktop/9315/postgresql-11.3/src/tutorial/email' LANGUAGE C IMMUTABLE STRICT;


CREATE OPERATOR < (
   leftarg = EmailAddr, 
   rightarg = EmailAddr, 
   procedure = email_lt,
   commutator = > , 
   negator = >= ,
   restrict = scalarltsel, 
   join = scalarltjoinsel
);

CREATE OPERATOR <= (
   leftarg = EmailAddr, 
   rightarg = EmailAddr, 
   procedure = email_le,
   commutator = >=,
   negator = > ,
   restrict = scalarlesel,
   join = scalarlejoinsel
);
CREATE OPERATOR = (
   leftarg = EmailAddr,
   rightarg = EmailAddr, 
   procedure = email_eq,
   commutator = =,
   negator  = <>,
   restrict = eqsel,
   join = eqjoinsel,MERGES,HASHES
);
CREATE OPERATOR <> (
   leftarg = EmailAddr,
   rightarg = EmailAddr,
   procedure = email_ne,
   commutator = <> ,
   negator = = ,
   restrict = neqsel,
   join = neqjoinsel
);
CREATE OPERATOR >= (
   leftarg = EmailAddr,
   rightarg = EmailAddr, 
   procedure = email_ge,
   commutator = <= ,
   negator = < ,
   restrict = scalargesel, 
   join = scalargejoinsel
);
CREATE OPERATOR > (
   leftarg = EmailAddr,
   rightarg = EmailAddr,
   procedure = email_gt,
   commutator = < , 
   negator = <= ,
   restrict = scalargtsel, 
   join = scalargtjoinsel
);
CREATE OPERATOR ~ (
   leftarg = EmailAddr, 
   rightarg = EmailAddr, 
   procedure = email_de,
   commutator = ~ , 
   negator = !~,
   restrict = eqsel, 
   join = eqjoinsel
);
CREATE OPERATOR !~ (
   leftarg = EmailAddr, 
   rightarg = EmailAddr, 
   procedure = email_dne,
   commutator = !~ , 
   negator = ~,
   restrict = neqsel, 
   join = neqjoinsel
);
/*CREATE OPERATOR CLASS email_ops DEFAULT FOR TYPE EmailAddr USING hash AS 
      OPERATOR 1  = ,
      FUNCTION 1  email_hash(EmailAddr);
*/
CREATE OPERATOR CLASS btree_email_ops
DEFAULT FOR TYPE EmailAddr USING btree
AS
        OPERATOR        1       <  ,
        OPERATOR        2       <= ,
        OPERATOR        3       =  ,
        OPERATOR        4       >= ,
        OPERATOR        5       >  ,
        FUNCTION        1       email_cmp(EmailAddr, EmailAddr);
CREATE OPERATOR CLASS email_ops DEFAULT FOR TYPE EmailAddr USING hash AS
      OPERATOR 1  = ,
      FUNCTION 1  email_hash(EmailAddr);
