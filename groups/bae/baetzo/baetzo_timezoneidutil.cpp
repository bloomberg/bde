// baetzo_timezoneidutil.cpp                                          -*-C++-*-
#include <baetzo_timezoneidutil.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baetzo_timezoneidutil_cpp,"$Id$ $CSID$")

///Implementation Notes
///--------------------
// This implementation uses standard algorithms to search two sorted statically
// defined tables: one mapping Windows timezone identifiers to Zoneinfo
// time-zone identifiers, and the other providing the inverse mapping.  The
// source for these tables was
// 'http://unicode.org/repos/cldr-tmp/trunk/diff/supplemental/zone_tzid.html',
// The entries were sorted using the 'isLessThan' function defined below as the
// comparator.  Furthermore, the implementation assumes that each table has
// only one entry for each given timezone identifier.  (That is, there is a
// one-to-one relationship between the two sets of timezone identifiers.
//
// This implementation was deemed to provide sufficient performance.  If a
// faster implementation is needed, we can take advantage of the fact that the
// table entries are known at compile-time.  Consider using a prefix tree
// (trie) or generating a perfect hash.

#include <baetzo_timezoneutil.h>  // for testing only
#include <baetzo_testloader.h>    // for testing only

#include <bsl_algorithm.h>  // for 'lower_bound'
#include <bsl_cstring.h>    // for 'strcmp'
#include <bslmf_assert.h>
#include <bsls_assert.h>

namespace BloombergLP {

typedef struct TimeZoneIdEntry {
    const char *d_key;
    const char *d_value;
} TimeZoneIdEntry;

typedef bsl::pair<const TimeZoneIdEntry *,
                  const TimeZoneIdEntry *> IteratorPair;

static bool isLessThan(const TimeZoneIdEntry& a,
                       const TimeZoneIdEntry& b)
    // Return 'true' is the "key" field of the specified 'a' is lexically
    // less that the "key" field of the specified 'b', and 'false otherwise.
{
    return bsl::strcmp(a.d_key, b.d_key) < 0;
}

static const TimeZoneIdEntry windowsToZoneinfoIds[] = {
    { "AUS Central Standard Time",       "Australia/Darwin"     },
    { "AUS Eastern Standard Time",       "Australia/Sydney"     },
    { "Afghanistan Standard Time",       "Asia/Kabul"           },
    { "Alaskan Standard Time",           "America/Anchorage"    },
    { "Arab Standard Time",              "Asia/Riyadh"          },
    { "Arabian Standard Time",           "Asia/Dubai"           },
    { "Arabic Standard Time",            "Asia/Baghdad"         },
    { "Argentina Standard Time",         "America/Buenos_Aires" },
    { "Atlantic Standard Time",          "America/Halifax"      },
    { "Azerbaijan Standard Time",        "Asia/Baku"            },
    { "Azores Standard Time",            "Atlantic/Azores"      },
    { "Bahia Standard Time",             "America/Bahia"        },
    { "Bangladesh Standard Time",        "Asia/Dhaka"           },
    { "Canada Central Standard Time",    "America/Regina"       },
    { "Cape Verde Standard Time",        "Atlantic/Cape_Verde"  },
    { "Caucasus Standard Time",          "Asia/Yerevan"         },
    { "Cen. Australia Standard Time",    "Australia/Adelaide"   },
    { "Central America Standard Time",   "America/Guatemala"    },
    { "Central Asia Standard Time",      "Asia/Almaty"          },
    { "Central Brazilian Standard Time", "America/Cuiaba"       },
    { "Central Europe Standard Time",    "Europe/Budapest"      },
    { "Central European Standard Time",  "Europe/Warsaw"        },
    { "Central Pacific Standard Time",   "Pacific/Guadalcanal"  },
    { "Central Standard Time",           "America/Chicago"      },
    { "Central Standard Time (Mexico)",  "America/Mexico_City"  },
    { "China Standard Time",             "Asia/Shanghai"        },
    { "Dateline Standard Time",          "Etc/GMT+12"           },
    { "E. Africa Standard Time",         "Africa/Nairobi"       },
    { "E. Australia Standard Time",      "Australia/Brisbane"   },
    { "E. Europe Standard Time",         "Asia/Nicosia"         },
    { "E. South America Standard Time",  "America/Sao_Paulo"    },
    { "Eastern Standard Time",           "America/New_York"     },
    { "Egypt Standard Time",             "Africa/Cairo"         },
    { "Ekaterinburg Standard Time",      "Asia/Yekaterinburg"   },
    { "FLE Standard Time",               "Europe/Kiev"          },
    { "Fiji Standard Time",              "Pacific/Fiji"         },
    { "GMT Standard Time",               "Europe/London"        },
    { "GTB Standard Time",               "Europe/Bucharest"     },
    { "Georgian Standard Time",          "Asia/Tbilisi"         },
    { "Greenland Standard Time",         "America/Godthab"      },
    { "Greenwich Standard Time",         "Atlantic/Reykjavik"   },
    { "Hawaiian Standard Time",          "Pacific/Honolulu"     },
    { "India Standard Time",             "Asia/Calcutta"        },
    { "Iran Standard Time",              "Asia/Tehran"          },
    { "Israel Standard Time",            "Asia/Jerusalem"       },
    { "Jordan Standard Time",            "Asia/Amman"           },
    { "Kaliningrad Standard Time",       "Europe/Kaliningrad"   },
    { "Korea Standard Time",             "Asia/Seoul"           },
    { "Magadan Standard Time",           "Asia/Magadan"         },
    { "Mauritius Standard Time",         "Indian/Mauritius"     },
    { "Middle East Standard Time",       "Asia/Beirut"          },
    { "Montevideo Standard Time",        "America/Montevideo"   },
    { "Morocco Standard Time",           "Africa/Casablanca"    },
    { "Mountain Standard Time",          "America/Denver"       },
    { "Mountain Standard Time (Mexico)", "America/Chihuahua"    },
    { "Myanmar Standard Time",           "Asia/Rangoon"         },
    { "N. Central Asia Standard Time",   "Asia/Novosibirsk"     },
    { "Namibia Standard Time",           "Africa/Windhoek"      },
    { "Nepal Standard Time",             "Asia/Katmandu"        },
    { "New Zealand Standard Time",       "Pacific/Auckland"     },
    { "Newfoundland Standard Time",      "America/St_Johns"     },
    { "North Asia East Standard Time",   "Asia/Irkutsk"         },
    { "North Asia Standard Time",        "Asia/Krasnoyarsk"     },
    { "Pacific SA Standard Time",        "America/Santiago"     },
    { "Pacific Standard Time",           "America/Los_Angeles"  },
    { "Pacific Standard Time (Mexico)",  "America/Santa_Isabel" },
    { "Pakistan Standard Time",          "Asia/Karachi"         },
    { "Paraguay Standard Time",          "America/Asuncion"     },
    { "Romance Standard Time",           "Europe/Paris"         },
    { "Russian Standard Time",           "Europe/Moscow"        },
    { "SA Eastern Standard Time",        "America/Cayenne"      },
    { "SA Pacific Standard Time",        "America/Bogota"       },
    { "SA Western Standard Time",        "America/La_Paz"       },
    { "SE Asia Standard Time",           "Asia/Bangkok"         },
    { "Samoa Standard Time",             "Pacific/Apia"         },
    { "Singapore Standard Time",         "Asia/Singapore"       },
    { "South Africa Standard Time",      "Africa/Johannesburg"  },
    { "Sri Lanka Standard Time",         "Asia/Colombo"         },
    { "Syria Standard Time",             "Asia/Damascus"        },
    { "Taipei Standard Time",            "Asia/Taipei"          },
    { "Tasmania Standard Time",          "Australia/Hobart"     },
    { "Tokyo Standard Time",             "Asia/Tokyo"           },
    { "Tonga Standard Time",             "Pacific/Tongatapu"    },
    { "Turkey Standard Time",            "Europe/Istanbul"      },
    { "US Eastern Standard Time",        "America/Indianapolis" },
    { "US Mountain Standard Time",       "America/Phoenix"      },
    { "UTC",                             "Etc/GMT"              },
    { "UTC+12",                          "Etc/GMT-12"           },
    { "UTC-02",                          "Etc/GMT+2"            },
    { "UTC-11",                          "Etc/GMT+11"           },
    { "Ulaanbaatar Standard Time",       "Asia/Ulaanbaatar"     },
    { "Venezuela Standard Time",         "America/Caracas"      },
    { "Vladivostok Standard Time",       "Asia/Vladivostok"     },
    { "W. Australia Standard Time",      "Australia/Perth"      },
    { "W. Central Africa Standard Time", "Africa/Lagos"         },
    { "W. Europe Standard Time",         "Europe/Berlin"        },
    { "West Asia Standard Time",         "Asia/Tashkent"        },
    { "West Pacific Standard Time",      "Pacific/Port_Moresby" },
    { "Yakutsk Standard Time",           "Asia/Yakutsk"         },
};

static const int numWindowsToZoneinfoIds = sizeof( windowsToZoneinfoIds)
                                         / sizeof(*windowsToZoneinfoIds);

static const TimeZoneIdEntry * const windowsToZoneinfoIdsBegin =
                                                          windowsToZoneinfoIds;

static const TimeZoneIdEntry * const windowsToZoneinfoIdsEnd   =
                                                          windowsToZoneinfoIds
                                                     + numWindowsToZoneinfoIds;

static const TimeZoneIdEntry zoneinfoToWindowsIds[] = {
    { "Africa/Cairo",         "Egypt Standard Time"             },
    { "Africa/Casablanca",    "Morocco Standard Time"           },
    { "Africa/Johannesburg",  "South Africa Standard Time"      },
    { "Africa/Lagos",         "W. Central Africa Standard Time" },
    { "Africa/Nairobi",       "E. Africa Standard Time"         },
    { "Africa/Windhoek",      "Namibia Standard Time"           },
    { "America/Anchorage",    "Alaskan Standard Time"           },
    { "America/Asuncion",     "Paraguay Standard Time"          },
    { "America/Bahia",        "Bahia Standard Time"             },
    { "America/Bogota",       "SA Pacific Standard Time"        },
    { "America/Buenos_Aires", "Argentina Standard Time"         },
    { "America/Caracas",      "Venezuela Standard Time"         },
    { "America/Cayenne",      "SA Eastern Standard Time"        },
    { "America/Chicago",      "Central Standard Time"           },
    { "America/Chihuahua",    "Mountain Standard Time (Mexico)" },
    { "America/Cuiaba",       "Central Brazilian Standard Time" },
    { "America/Denver",       "Mountain Standard Time"          },
    { "America/Godthab",      "Greenland Standard Time"         },
    { "America/Guatemala",    "Central America Standard Time"   },
    { "America/Halifax",      "Atlantic Standard Time"          },
    { "America/Indianapolis", "US Eastern Standard Time"        },
    { "America/La_Paz",       "SA Western Standard Time"        },
    { "America/Los_Angeles",  "Pacific Standard Time"           },
    { "America/Mexico_City",  "Central Standard Time (Mexico)"  },
    { "America/Montevideo",   "Montevideo Standard Time"        },
    { "America/New_York",     "Eastern Standard Time"           },
    { "America/Phoenix",      "US Mountain Standard Time"       },
    { "America/Regina",       "Canada Central Standard Time"    },
    { "America/Santa_Isabel", "Pacific Standard Time (Mexico)"  },
    { "America/Santiago",     "Pacific SA Standard Time"        },
    { "America/Sao_Paulo",    "E. South America Standard Time"  },
    { "America/St_Johns",     "Newfoundland Standard Time"      },
    { "Asia/Almaty",          "Central Asia Standard Time"      },
    { "Asia/Amman",           "Jordan Standard Time"            },
    { "Asia/Baghdad",         "Arabic Standard Time"            },
    { "Asia/Baku",            "Azerbaijan Standard Time"        },
    { "Asia/Bangkok",         "SE Asia Standard Time"           },
    { "Asia/Beirut",          "Middle East Standard Time"       },
    { "Asia/Calcutta",        "India Standard Time"             },
    { "Asia/Colombo",         "Sri Lanka Standard Time"         },
    { "Asia/Damascus",        "Syria Standard Time"             },
    { "Asia/Dhaka",           "Bangladesh Standard Time"        },
    { "Asia/Dubai",           "Arabian Standard Time"           },
    { "Asia/Irkutsk",         "North Asia East Standard Time"   },
    { "Asia/Jerusalem",       "Israel Standard Time"            },
    { "Asia/Kabul",           "Afghanistan Standard Time"       },
    { "Asia/Karachi",         "Pakistan Standard Time"          },
    { "Asia/Katmandu",        "Nepal Standard Time"             },
    { "Asia/Krasnoyarsk",     "North Asia Standard Time"        },
    { "Asia/Magadan",         "Magadan Standard Time"           },
    { "Asia/Nicosia",         "E. Europe Standard Time"         },
    { "Asia/Novosibirsk",     "N. Central Asia Standard Time"   },
    { "Asia/Rangoon",         "Myanmar Standard Time"           },
    { "Asia/Riyadh",          "Arab Standard Time"              },
    { "Asia/Seoul",           "Korea Standard Time"             },
    { "Asia/Shanghai",        "China Standard Time"             },
    { "Asia/Singapore",       "Singapore Standard Time"         },
    { "Asia/Taipei",          "Taipei Standard Time"            },
    { "Asia/Tashkent",        "West Asia Standard Time"         },
    { "Asia/Tbilisi",         "Georgian Standard Time"          },
    { "Asia/Tehran",          "Iran Standard Time"              },
    { "Asia/Tokyo",           "Tokyo Standard Time"             },
    { "Asia/Ulaanbaatar",     "Ulaanbaatar Standard Time"       },
    { "Asia/Vladivostok",     "Vladivostok Standard Time"       },
    { "Asia/Yakutsk",         "Yakutsk Standard Time"           },
    { "Asia/Yekaterinburg",   "Ekaterinburg Standard Time"      },
    { "Asia/Yerevan",         "Caucasus Standard Time"          },
    { "Atlantic/Azores",      "Azores Standard Time"            },
    { "Atlantic/Cape_Verde",  "Cape Verde Standard Time"        },
    { "Atlantic/Reykjavik",   "Greenwich Standard Time"         },
    { "Australia/Adelaide",   "Cen. Australia Standard Time"    },
    { "Australia/Brisbane",   "E. Australia Standard Time"      },
    { "Australia/Darwin",     "AUS Central Standard Time"       },
    { "Australia/Hobart",     "Tasmania Standard Time"          },
    { "Australia/Perth",      "W. Australia Standard Time"      },
    { "Australia/Sydney",     "AUS Eastern Standard Time"       },
    { "Etc/GMT",              "UTC"                             },
    { "Etc/GMT+11",           "UTC-11"                          },
    { "Etc/GMT+12",           "Dateline Standard Time"          },
    { "Etc/GMT+2",            "UTC-02"                          },
    { "Etc/GMT-12",           "UTC+12"                          },
    { "Europe/Berlin",        "W. Europe Standard Time"         },
    { "Europe/Bucharest",     "GTB Standard Time"               },
    { "Europe/Budapest",      "Central Europe Standard Time"    },
    { "Europe/Istanbul",      "Turkey Standard Time"            },
    { "Europe/Kaliningrad",   "Kaliningrad Standard Time"       },
    { "Europe/Kiev",          "FLE Standard Time"               },
    { "Europe/London",        "GMT Standard Time"               },
    { "Europe/Moscow",        "Russian Standard Time"           },
    { "Europe/Paris",         "Romance Standard Time"           },
    { "Europe/Warsaw",        "Central European Standard Time"  },
    { "Indian/Mauritius",     "Mauritius Standard Time"         },
    { "Pacific/Apia",         "Samoa Standard Time"             },
    { "Pacific/Auckland",     "New Zealand Standard Time"       },
    { "Pacific/Fiji",         "Fiji Standard Time"              },
    { "Pacific/Guadalcanal",  "Central Pacific Standard Time"   },
    { "Pacific/Honolulu",     "Hawaiian Standard Time"          },
    { "Pacific/Port_Moresby", "West Pacific Standard Time"      },
    { "Pacific/Tongatapu",    "Tonga Standard Time"             },
};

static const int numZoneinfoToWindowsIds = sizeof( zoneinfoToWindowsIds)
                                         / sizeof(*zoneinfoToWindowsIds);

static const TimeZoneIdEntry * const zoneinfoToWindowsIdsBegin =
                                                          zoneinfoToWindowsIds;
static const TimeZoneIdEntry * const zoneinfoToWindowsIdsEnd  =
                                                          zoneinfoToWindowsIds
                                                     + numZoneinfoToWindowsIds;

namespace {

BSLMF_ASSERT(                     99 == numWindowsToZoneinfoIds);
BSLMF_ASSERT(numWindowsToZoneinfoIds == numZoneinfoToWindowsIds);

}  // close unnamed namespace

                        // ---------------------------
                        // class baetzo_TimeZoneUtilId
                        // ---------------------------

// CLASS METHODS
#if 0
    static int zoneinfoIdFromWindowsTimeZoneId(const char **result,
                                               const char  *windowsTimeZoneId);
        // Load into the specified 'result' the address of the default Zoneinfo
        // time-zone identifier for the specified 'windowsTimeZoneId'.  Return
        // 0 on success, and non-zero value with no other effect otherwise.
        // The returned address is valid for the life-time of the process.

    static int windowsTimeZoneIdFromZoneinfoId(const char **result,
                                               const char  *zoneinfoId);
        // Load into the specified 'result' the address the Windows time-zone
        // identifier that has a default mapping to the specified 'zoneinfoId'.
        // Return 0 on success, and non-zero value with no other effect
        // otherwise.  The returned address is valid for the life-time of the
        // process.
#endif
int baetzo_TimeZoneIdUtil::zoneinfoIdFromWindowsTimeZoneId(
                                                const char **result,
                                                const char  *windowsTimeZoneId)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(windowsTimeZoneId);

    const TimeZoneIdEntry value = { windowsTimeZoneId, 0 };

    IteratorPair iterators = bsl::equal_range(windowsToZoneinfoIdsBegin,
                                              windowsToZoneinfoIdsEnd,
                                              value,
                                              isLessThan);

    if (iterators.first == iterators.second) {
        return -1;                                                    // RETURN
    }

    BSLS_ASSERT_SAFE(1 == bsl::distance(iterators.first, iterators.second));

    *result = iterators.first->d_value;
    return 0;
}

int baetzo_TimeZoneIdUtil::windowsTimeZoneIdFromZoneinfoId(
                                                       const char **result,
                                                       const char  *zoneinfoId)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(zoneinfoId);

    const TimeZoneIdEntry value = { zoneinfoId, 0 };

    IteratorPair iterators = bsl::equal_range(zoneinfoToWindowsIdsBegin,
                                              zoneinfoToWindowsIdsEnd,
                                              value,
                                              isLessThan);

    if (iterators.first == iterators.second) {
        return -1;                                                    // RETURN
    }

    BSLS_ASSERT_SAFE(1 == bsl::distance(iterators.first, iterators.second));

    *result = iterators.first->d_value;
    return 0;
}

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
