using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;

namespace UpdateManager.Entities
{
    public class UpdatePackage
    {
        #region Properties
        public int Id { get; set; }
        
        public int UpdateId { get; set; }
        
        public DateTime CreationDate { get; set; }

        public UpdatePackagePriority Priority { get; set; }

        public DateTime UpdateDate { get; set; }

        [Required]
        [StringLength(255)]
        public string CreatedBy { get; set; }

        public virtual ICollection<PackageFile> Files { get; set; }
        #endregion
    }
}
