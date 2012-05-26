// baetzo_timezoneidutil.cpp                                          -*-C++-*-
#include <baetzo_timezoneidutil.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baetzo_timezoneidutil_cpp,"$Id$ $CSID$")

#include <bsl_algorithm.h>  // for 'lower_bound'
#include <bsl_cstring.h>    // for 'strcmp'
#include <bslmf_assert.h>
#include <bsls_assert.h>

#include <bsl_iostream.h>   // debug

namespace BloombergLP {

typedef struct TimeZoneIdEntry {
    const char *d_fromTimeZoneId;
    const char *d_toTimeZoneId;
} TimeZoneIdEntry;

static bool compareTimeZoneIdEntries(const TimeZoneIdEntry& a,
                                     const TimeZoneIdEntry& b)
{
    bsl::cout << "compareTimeZoneIdEntries: enter: "
              << a.d_fromTimeZoneId << ": "
              << b.d_fromTimeZoneId << bsl::endl;
    bool ret =  -1 == bsl::strcmp(a.d_fromTimeZoneId, b.d_fromTimeZoneId);
    bsl::cout << "compareTimeZoneIdEntries: leave: " << ret << bsl::endl;
    return ret;
}

static const TimeZoneIdEntry windowsToOlsonIds[] = {
    { "Afghanistan Standard Time",       "Asia/Kabul"           },
    { "Alaskan Standard Time",           "America/Anchorage"    },
    { "Arabian Standard Time",           "Asia/Dubai"           },
    { "Arabic Standard Time",            "Asia/Baghdad"         },
    { "Arab Standard Time",              "Asia/Riyadh"          },
    { "Argentina Standard Time",         "America/Buenos_Aires" },
    { "Atlantic Standard Time",          "America/Halifax"      },
    { "AUS Central Standard Time",       "Australia/Darwin"     },
    { "AUS Eastern Standard Time",       "Australia/Sydney"     },
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
    { "Central European Standard Time",  "Europe/Warsaw"        },
    { "Central Europe Standard Time",    "Europe/Budapest"      },
    { "Central Pacific Standard Time",   "Pacific/Guadalcanal"  },
    { "Central Standard Time",           "America/Chicago"      },
    { "Central Standard Time (Mexico)",  "America/Mexico_City"  },
    { "China Standard Time",             "Asia/Shanghai"        },
    { "Dateline Standard Time",          "Etc/GMT+12"           },
    { "E. Africa Standard Time",         "Africa/Nairobi"       },
    { "Eastern Standard Time",           "America/New_York"     },
    { "E. Australia Standard Time",      "Australia/Brisbane"   },
    { "E. Europe Standard Time",         "Asia/Nicosia"         },
    { "Egypt Standard Time",             "Africa/Cairo"         },
    { "Ekaterinburg Standard Time",      "Asia/Yekaterinburg"   },
    { "E. South America Standard Time",  "America/Sao_Paulo"    },
    { "Fiji Standard Time",              "Pacific/Fiji"         },
    { "FLE Standard Time",               "Europe/Kiev"          },
    { "Georgian Standard Time",          "Asia/Tbilisi"         },
    { "GMT Standard Time",               "Europe/London"        },
    { "Greenland Standard Time",         "America/Godthab"      },
    { "Greenwich Standard Time",         "Atlantic/Reykjavik"   },
    { "GTB Standard Time",               "Europe/Bucharest"     },
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
    { "Namibia Standard Time",           "Africa/Windhoek"      },
    { "N. Central Asia Standard Time",   "Asia/Novosibirsk"     },
    { "Nepal Standard Time",             "Asia/Katmandu"        },
    { "Newfoundland Standard Time",      "America/St_Johns"     },
    { "New Zealand Standard Time",       "Pacific/Auckland"     },
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
    { "Samoa Standard Time",             "Pacific/Apia"         },
    { "SA Pacific Standard Time",        "America/Bogota"       },
    { "SA Western Standard Time",        "America/La_Paz"       },
    { "SE Asia Standard Time",           "Asia/Bangkok"         },
    { "Singapore Standard Time",         "Asia/Singapore"       },
    { "South Africa Standard Time",      "Africa/Johannesburg"  },
    { "Sri Lanka Standard Time",         "Asia/Colombo"         },
    { "Syria Standard Time",             "Asia/Damascus"        },
    { "Taipei Standard Time",            "Asia/Taipei"          },
    { "Tasmania Standard Time",          "Australia/Hobart"     },
    { "Tokyo Standard Time",             "Asia/Tokyo"           },
    { "Tonga Standard Time",             "Pacific/Tongatapu"    },
    { "Turkey Standard Time",            "Europe/Istanbul"      },
    { "Ulaanbaatar Standard Time",       "Asia/Ulaanbaatar"     },
    { "US Eastern Standard Time",        "America/Indianapolis" },
    { "US Mountain Standard Time",       "America/Phoenix"      },
    { "UTC-02",                          "Etc/GMT+2"            },
    { "UTC-11",                          "Etc/GMT+11"           },
    { "UTC+12",                          "Etc/GMT-12"           },
    { "UTC",                             "Etc/GMT"              },
    { "Venezuela Standard Time",         "America/Caracas"      },
    { "Vladivostok Standard Time",       "Asia/Vladivostok"     },
    { "W. Australia Standard Time",      "Australia/Perth"      },
    { "W. Central Africa Standard Time", "Africa/Lagos"         },
    { "West Asia Standard Time",         "Asia/Tashkent"        },
    { "West Pacific Standard Time",      "Pacific/Port_Moresby" },
    { "W. Europe Standard Time",         "Europe/Berlin"        },
    { "Yakutsk Standard Time",           "Asia/Yakutsk"         }
};

static const int numWindowsToOlsonIds = sizeof( windowsToOlsonIds)
                                      / sizeof(*windowsToOlsonIds);

static const TimeZoneIdEntry * const windowsToOlsonIdsBegin = windowsToOlsonIds;
static const TimeZoneIdEntry * const windowsToOlsonIdsEnd   = windowsToOlsonIds
                                                        + numWindowsToOlsonIds;

static const struct TimeZoneIdEntry olsonToWindowsIds[] = {
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
    { "Etc/GMT+11",           "UTC-11"                          },
    { "Etc/GMT+12",           "Dateline Standard Time"          },
    { "Etc/GMT-12",           "UTC+12"                          },
    { "Etc/GMT+2",            "UTC-02"                          },
    { "Etc/GMT",              "UTC"                             },
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
    { "Pacific/Tongatapu",    "Tonga Standard Time"             }
};

static const int numOlsonToWindowsIds = sizeof( olsonToWindowsIds)
                                      / sizeof(*olsonToWindowsIds);

static const TimeZoneIdEntry * const olsonToWindowsIdsBegin =
                                                             olsonToWindowsIds;
static const TimeZoneIdEntry * const olsonToWindowsIdsEnd   = olsonToWindowsIds
                                                      + numOlsonToWindowsIds;

namespace {

BSLMF_ASSERT(                  99 == numWindowsToOlsonIds);
BSLMF_ASSERT(numWindowsToOlsonIds == numOlsonToWindowsIds);

}

                        // ---------------------------
                        // class baetzo_TimeZoneUtilId
                        // ---------------------------

// CLASS METHODS

int baetzo_TimeZoneIdUtil::getTimeZoneIdFromWindowsTimeZoneId(
                                                const char **timeZoneId,
                                                const char  *windowsTimeZoneId)
{
    BSLS_ASSERT(timeZoneId);
    BSLS_ASSERT(windowsTimeZoneId);

    bsl::cout << "getTimeZoneIdFromWindowsTimeZoneId: enter: "
              << windowsTimeZoneId
              << bsl::endl;

    TimeZoneIdEntry value = { windowsTimeZoneId, 0 };

    TimeZoneIdEntry *ptr = bsl::lower_bound( // TBD: why 'const_cast' needed?
                          const_cast<TimeZoneIdEntry*>(windowsToOlsonIdsBegin),
                          const_cast<TimeZoneIdEntry*>(windowsToOlsonIdsEnd),
                          value,
                          compareTimeZoneIdEntries);
    if (windowsToOlsonIdsEnd == ptr) {
        bsl::cout << "getTimeZoneIdFromWindowsTimeZoneId: leave: " << "error"
                  << ptr->d_toTimeZoneId
                  << bsl::endl;
        return -1;                                                    // RETURN
    }

    bsl::cout << "getTimeZoneIdFromWindowsTimeZoneId: leave: "
              << ptr->d_toTimeZoneId
              << bsl::endl;

    *timeZoneId = ptr->d_toTimeZoneId;
    return 0;
}


int baetzo_TimeZoneIdUtil::getWindowsTimeZoneIdFromTimeZoneId(
                                                const char **windowsTimeZoneId,
                                                const char  *timeZoneId)
{
    BSLS_ASSERT(windowsTimeZoneId);
    BSLS_ASSERT(timeZoneId);

    bsl::cout << "getWindowsTimeZoneIdFromTimeZoneId: enter: "
              << timeZoneId
              << bsl::endl;

    TimeZoneIdEntry value = { timeZoneId, 0 };

    TimeZoneIdEntry *ptr = bsl::lower_bound( // TBD: why 'const_cast' needed?
                          const_cast<TimeZoneIdEntry*>(olsonToWindowsIdsBegin),
                          const_cast<TimeZoneIdEntry*>(olsonToWindowsIdsEnd  ),
                          value,
                          compareTimeZoneIdEntries);
    if (olsonToWindowsIdsEnd == ptr) {
        bsl::cout << "getWindowsTimeZoneIdFromTimeZoneId: leave: " << "error"
                  << ptr->d_toTimeZoneId
                  << bsl::endl;
        return -1;                                                    // RETURN
    }

    bsl::cout << "getWindowsTimeZoneIdFromTimeZoneId: leave: "
              << ptr->d_toTimeZoneId
              << bsl::endl;

    *windowsTimeZoneId = ptr->d_toTimeZoneId;
    return 0;
}

}  // close namespace BloombergLP

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
