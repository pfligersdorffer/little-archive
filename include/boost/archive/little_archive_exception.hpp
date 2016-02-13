/*****************************************************************************/
/**
 * \file little_archive_exception.hpp
 * \brief Provides error handling and constants.
 * \author christian.pfligersdorffer@gmx.at
 *
 * little archive exceptions derive from the boost archive exceptions
 * and add failure causes specific to the portable binary usecase.
 *
 * Additionally this header serves as common include for important
 * constants or typedefs.
 */
/****************************************************************************/

#pragma once

#include <boost/lexical_cast.hpp>
#include <boost/archive/basic_archive.hpp>
#include <boost/archive/archive_exception.hpp>

// hint from Johan Rade: on VMS there is still support for
// the VAX floating point format and this macro detects it
#if defined(__vms) && defined(__DECCXX) && !__IEEE_FLOAT
#error "VAX floating point format is not supported!"
#endif

namespace boost {
	namespace archive {

		// this value is written to the top of the stream
		const signed char magic_byte = 127;

		// flag for fp serialization
		const unsigned no_infnan = 64;

		// integral type for the archive version
		typedef library_version_type archive_version_type;

		// version of the linked boost archive library
		const archive_version_type archive_version(BOOST_ARCHIVE_VERSION());

		/**
		 * \brief Exception being thrown when serialization cannot proceed.
		 *
		 * There are several situations in which the little archives may fail and
		 * hence throw an exception:
		 * -# deserialization of an integer value that exceeds the range of the type
		 * -# (de)serialization of inf/nan through an archive with no_infnan flag set
		 * -# deserialization of a denormalized value without the floating point type
		 *    supporting denormalized numbers
		 *
		 * Note that this exception will also be thrown if you mixed up your stream
		 * position and accidentially interpret some value for size data (in this case
		 * the reported size will be totally amiss most of the time).
		 */
		class little_archive_exception : public archive_exception
		{
			std::string msg;

		public:
			//! type size is not large enough for deserialized number
			little_archive_exception(signed char invalid_size)
				: archive_exception(other_exception)
				, msg("requested integer size exceeds type size: ")
			{
				msg += lexical_cast<std::string, int>(invalid_size);
			}

			//! negative number in unsigned type
			little_archive_exception()
				: archive_exception(other_exception)
				, msg("cannot read a negative number into an unsigned type")
			{
			}

			//! serialization of inf, nan and denormals
			template <typename T>
			little_archive_exception(const T& abnormal)
				: archive_exception(other_exception)
				, msg("serialization of illegal floating point value: ")
			{
				msg += lexical_cast<std::string>(abnormal);
			}

			//! override the base class function with our message
			const char* what() const throw() { return msg.c_str(); }
			~little_archive_exception() throw() {}
		};

	} // namespace archive
} // namespace boost
