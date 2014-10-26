using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace UpdateManager.Entities
{
    public enum UpdateType
    {
        [Display(Name="Major")]
        MAJOR,
        MINOR,
        PATCH
    }

    public enum UpdatePackagePriority
    {
        HIGH,
        MEDIUM,
        LOW
    }

    public enum UpdateStatus
    {        
        DRAFT,
        PUBLISHED,
        NONE
    }
}
