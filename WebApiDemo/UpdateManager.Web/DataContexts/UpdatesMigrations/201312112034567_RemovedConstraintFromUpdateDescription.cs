namespace UpdateManager.Web.DataContexts.UpdatesMigrations
{
    using System;
    using System.Data.Entity.Migrations;
    
    public partial class RemovedConstraintFromUpdateDescription : DbMigration
    {
        public override void Up()
        {
            AlterColumn("UpdateManager.Updates", "Description", c => c.String(nullable: false));
        }
        
        public override void Down()
        {
            AlterColumn("UpdateManager.Updates", "Description", c => c.String(nullable: false, maxLength: 500));
        }
    }
}
