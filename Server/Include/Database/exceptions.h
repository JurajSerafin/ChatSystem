#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

/**
 * @brief Convenience header aggregating all database-related exception types.
 * * Including this single file provides access to the complete hierarchy of
 * exceptions thrown by the database repository layer.
 */

#include <Database/Exceptions/column_parse_exception.h>
#include <Database/Exceptions/column_type_mismatch_exception.h>
#include <Database/Exceptions/connection_exception.h>
#include <Database/Exceptions/constraint_exception.h>
#include <Database/Exceptions/not_found_exception.h>
#include <Database/Exceptions/query_exception.h>
#include <Database/Exceptions/transaction_exception.h>

#endif // EXCEPTIONS_H