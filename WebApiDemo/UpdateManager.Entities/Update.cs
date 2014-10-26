using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;

namespace UpdateManager.Entities
{
    public class Update
    {
        #region Properties
        public int Id { get; set; }

        [Required]
        [StringLength(200)]
        public string Name { get; set; }

        [Required]        
        public string Description { get; set; }

        [StringLength(500)]
        public string ChangeLogPath { get; set; }

        public UpdateType Type { get; set; }

        public UpdateStatus Status { get; set; }

        public DateTime CreationDate { get; set; }

        public DateTime PublishedDate { get; set; }

        [Required]
        [StringLength(255)]
        public string CreatedBy { get; set; }

        public virtual ICollection<UpdatePackage> Packages { get; set; }
        #endregion
    }
}
